#include "camera/Camera.hpp"

Camera::Camera(int width, int height) {
	m_width = width;
	m_height = height;
	libcameraCameraManager = std::make_unique<libcamera::CameraManager>(); // TODO: Needs to be executed only one time, put it elsewhere
}


int Camera::open(std::string source) {
	#ifdef ARM
		libcameraCameraManager->start();

		//Just as a test, generate names of the Cameras registered in the system, and list them.
		for (auto const &libcameraCamera : libcameraCameraManager->cameras())
			std::cout << " - " << libcameraCameraName(libcameraCamera.get()) << std::endl;

		if (libcameraCameraManager->cameras().empty()) {
			spdlog::error("No cameras were identified on the system.");
			libcameraCameraManager->stop();
			return EXIT_FAILURE;
		}

		std::string cameraId = libcameraCameraManager->cameras()[0]->id();
		libcameraCamera = libcameraCameraManager->get(cameraId); // Or more simply libcameraCameraManager->cameras()[0]
		libcameraCamera->acquire();
		
		libcameraCameraConfiguration = libcameraCamera->generateConfiguration( { libcamera::StreamRole::VideoRecording } ); //Viewfinder } );
		libcamera::StreamConfiguration &streamConfig = libcameraCameraConfiguration->at(0);
		spdlog::debug("Default viewfinder configuration is: {}", streamConfig.toString());	
		
		streamConfig.size.width = m_width; //4608 //4096 //m_width;
		streamConfig.size.height = m_height; //2592 //2560 //m_height;
		streamConfig.pixelFormat = libcamera::PixelFormat::fromString("RGB888");//SRGGB10_CSI2P");
		if (streamConfig.pixelFormat.isValid()) {
			spdlog::debug("Pixel format {} is valid", streamConfig.pixelFormat.toString()); 
		} else {
			spdlog::debug("Pixel format {} is invalid", streamConfig.pixelFormat.toString());
		}
		libcameraCameraConfiguration->validate();
			
		{
			int ret = libcameraCamera->configure(libcameraCameraConfiguration.get());
			if (ret) {
				spdlog::error("CONFIGURATION FAILED! RETURNED VALUE: {}", ret);
				return EXIT_FAILURE;
			} else {
				spdlog::info("Camera configuration done");
			}
		}

		libcameraCameraConfiguration->validate();
		spdlog::info("Validated viewfinder configuration is: {}", streamConfig.toString());
		libcameraCamera->configure(libcameraCameraConfiguration.get());

		allocatorPtr = new libcamera::FrameBufferAllocator(libcameraCamera);

		for (libcamera::StreamConfiguration &cfg : *libcameraCameraConfiguration) {
			int ret = allocatorPtr->allocate(cfg.stream());
			if (ret < 0) {
				spdlog::error("Can't allocate buffers");
				return EXIT_FAILURE;
			}

			size_t allocated = allocatorPtr->buffers(cfg.stream()).size();
			spdlog::debug("Allocated {} buffers for stream", allocated);
		}
		
		streamPtr = streamConfig.stream();
		
		const std::vector<std::unique_ptr<libcamera::FrameBuffer>> &buffers = allocatorPtr->buffers(streamPtr);
		for (unsigned int i = 0; i < buffers.size(); ++i) {
			std::unique_ptr<libcamera::Request> request = libcameraCamera->createRequest();
			if (!request) {
				spdlog::error("Can't create request");
				//return EXIT_FAILURE;
			}

			const std::unique_ptr<libcamera::FrameBuffer> &buffer = buffers[i];
			int ret = request->addBuffer(streamPtr, buffer.get());
			if (ret < 0) {
				spdlog::error("Can't set buffer for request {}", i);
				//return EXIT_FAILURE;
			}

			changeControls(request->controls());
			m_startingRequests.push_back(std::move(request));
		}
		
		libcameraCamera->requestCompleted.connect(requestComplete);
		
		m_intrinsicMatrix = cv::Mat(3, 3, CV_32FC1, new float[] { 1010.f, 0.f, m_width / 2.f, 0.f, 1010.f, m_height / 2.f, 0.f, 0.f, 1.f });
		m_distortionCoefficients = cv::Mat(5, 1, CV_32FC1, new float[] { 0, 0, 0, 0, 0 }); //-0.041f, 0.097f, -0.012f, 0.00026f, -0.066f}); // Maybe 5 rows and 1 column
	#else
		m_processingImageFile = false;
		if (std::find_if(source.begin(), source.end(), [](unsigned char c) { return !std::isdigit(c); }) == source.end()) {
			m_cap.open(atoi(source.c_str()));
		} else {
			m_filePath = source;
			if (m_filePath.extension() == ".png" || m_filePath.extension() == ".jpg" || m_filePath.extension() == ".jpeg") {
				m_processingImageFile = true;
			} else {
				m_cap.open(source);
			}
		}
		
		if (m_processingImageFile) {
			cv::Mat testFrame = cv::imread(m_filePath.string());
			if (testFrame.empty()) {
				spdlog::error("File image is empty or doesn't exist");
				return EXIT_FAILURE;
			}
			m_width = testFrame.cols; // In case of resolution slightly changed
			m_height = testFrame.rows;
		} else {
			if (!m_cap.isOpened()) {
				spdlog::error("No video stream detected");
				return EXIT_FAILURE;
			}
			m_cap.set(cv::CAP_PROP_FRAME_WIDTH, m_width);
			m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, m_height);
			cv::Mat testFrame;
			m_cap >> testFrame;
			if (testFrame.empty()) {
				spdlog::error("Test frame is empty");
				return EXIT_FAILURE;
			}
			m_width = testFrame.cols; // In case of resolution slightly changed
			m_height = testFrame.rows;
		}
		
		m_intrinsicMatrix = cv::Mat(3, 3, CV_32FC1, new float[] { 850.f, 0.f, m_width / 2.f, 0.f, 850.f, m_height / 2.f, 0.f, 0.f, 1.f });
		m_distortionCoefficients = cv::Mat(5, 1, CV_32FC1, new float[] { 0, 0, 0, 0, 0 }); // Maybe 5 rows and 1 column
	#endif //ARM

	spdlog::debug("Camera intrinsic matrix:\n{}", m_intrinsicMatrix);
	spdlog::debug("Camera distortion coefficients:\n{}", m_distortionCoefficients);
	
	return EXIT_SUCCESS;
}

void Camera::run() {
	#ifdef ARM
		static bool isFirstRun = true;
		if (isFirstRun) {
			libcameraCamera->start();
			isFirstRun = false;
			
			for (std::unique_ptr<libcamera::Request>& request : m_startingRequests)
				libcameraCamera->queueRequest(request.get());
		} else {
			std::vector<std::unique_ptr<libcamera::Request>> requests;
			const std::vector<std::unique_ptr<libcamera::FrameBuffer>>& buffers = allocatorPtr->buffers(streamPtr);
			for (unsigned int i = 0; i < buffers.size(); ++i) {
				std::unique_ptr<libcamera::Request> request = libcameraCamera->createRequest();
				if (!request) {
					spdlog::error("Can't create request");
					//return EXIT_FAILURE;
				}

				const std::unique_ptr<libcamera::FrameBuffer>& buffer = buffers[i];
				int ret = request->addBuffer(streamPtr, buffer.get());
				if (ret < 0) {
					spdlog::error("Can't set buffer for request {}", i);
					//return EXIT_FAILURE;
				}

				changeControls(request->controls());
				requests.push_back(std::move(request));
			}
			
			for (std::unique_ptr<libcamera::Request>& request: requests)
				libcameraCamera->queueRequest(request.get());
		}
		
		//TODO: Move this because it shouldn't be here
		int ret = loop.exec();
		spdlog::info("Capture stopped with exit status: {}", ret);
		
		//libcameraCamera->stop();
	#else
		cv::Mat inputFrame;
		bool running = true;
		while (running) {
			if (m_processingImageFile) {
				inputFrame = cv::imread(m_filePath.string());
			} else {
				m_cap >> inputFrame;
			}
			int returnCode = processFrameFunction(inputFrame);
			if (returnCode != EXIT_SUCCESS) {
				running = false;
			}
		}
	#endif //ARM
}

void Camera::close() {
	#ifdef ARM
		libcameraCamera->stop();
		allocatorPtr->free(streamPtr);
		delete allocatorPtr;
		libcameraCamera->release();
		libcameraCamera.reset();
		libcameraCameraManager->stop();
	#else
		if (m_cap.isOpened())
			m_cap.release();
	#endif //ARM
	
	#ifdef CVWINDOW_ENABLED 
		cv::destroyAllWindows();
		cv::waitKey(1);
	#endif // CVWINDOW_ENABLED
}

#ifdef ARM
	static void requestComplete(libcamera::Request *request) {
		if (request->status() == libcamera::Request::RequestCancelled)
			return;
		
		if (running)
			loop.callLater(std::bind(&processRequest, request));
	}
	
	static void processRequest(libcamera::Request *request) {
		static cv::Mat capturedMat = cv::Mat(0, 0, CV_8UC3);//;, stride)
		static int iteration = 0;
		spdlog::debug("Request completed: {}", request->toString());

		const libcamera::Request::BufferMap &buffers = request->buffers();
		for (auto bufferPair : buffers) {
			const libcamera::Stream *stream = bufferPair.first;
			libcamera::FrameBuffer *buffer = bufferPair.second;
			const libcamera::FrameMetadata &metadata = buffer->metadata();
			
			{
				std::string frameMetadataPlanesMessage = "Frame metadata planes: " + std::to_string(metadata.planes().size()) + ": ";
				unsigned int nplane = 0;
				for (const libcamera::FrameMetadata::Plane &plane : metadata.planes()) {
					frameMetadataPlanesMessage += std::to_string(plane.bytesused);
					if (++nplane < metadata.planes().size())
						frameMetadataPlanesMessage += "/";
				}
				spdlog::debug(frameMetadataPlanesMessage);
			}
			{
				const std::vector<libcamera::FrameBuffer::Plane>& planes = buffer->planes();
				std::string mappedBufferPlanesMessage = "Frame buffer planes: " + std::to_string(planes.size()) + ": ";
				unsigned int nplane = 0;
				for (const libcamera::FrameBuffer::Plane& plane: planes) {
					mappedBufferPlanesMessage += std::to_string(plane.length);
					if (++nplane < planes.size())
						mappedBufferPlanesMessage += "/";
				}
				spdlog::debug(mappedBufferPlanesMessage);
			}

			auto cfg = stream->configuration();
			unsigned int width = cfg.size.width;
			unsigned int height = cfg.size.height;
			unsigned int stride = cfg.stride;
			spdlog::debug("Size: {}x{}, stride: {}, format: {}", width, height, stride, cfg.pixelFormat.toString());

			libcamera::MappedFrameBuffer mappedBuffer(buffer, libcamera::MappedFrameBuffer::MapFlag::Read);
			const std::vector<libcamera::Span<uint8_t>> mem = mappedBuffer.planes();
			
			if (iteration == 0) {
				capturedMat.rows = height;
				capturedMat.cols = width;
				capturedMat.data = (uint8_t *)(mem[0].data());
				capturedMat.step = stride;
			}
			
			int returnCode = processFrameFunction(&capturedMat);
			
			if (returnCode != EXIT_SUCCESS) {
				running = false;
			}			
		}

		/* Re-queue the Request to the camera. */
		if (running) {
			request->reuse(libcamera::Request::ReuseBuffers);
			changeControls(request->controls());
			libcameraCamera->queueRequest(request);
		} else {
			loop.exit();
		}
		iteration++;
	}

	std::string libcameraCameraName(libcamera::Camera *libcameraCamera) {
		const libcamera::ControlList &props = libcameraCamera->properties();
		std::string name;

		const auto &location = props.get(libcamera::properties::Location);
		if (location) {
			switch (*location) {
			case libcamera::properties::CameraLocationFront:
				name = "Internal front camera";
				break;
			case libcamera::properties::CameraLocationBack:
				name = "Internal back camera";
				break;
			case libcamera::properties::CameraLocationExternal:
				name = "External camera";
				const auto &model = props.get(libcamera::properties::Model);
				if (model)
					name = " '" + *model + "'";
				break;
			}
		}
		name += " (" + libcameraCamera->id() + ")";
		return name;
	}
	
	std::int64_t frameDurationLimits[2] = {1000, 20000}; 
	void changeControls(libcamera::ControlList& controls) {
		//Controls can be added to a request on a per frame basis.
		static int iteration = 0;
		
		//controls.set(libcamera::controls::AeEnable, true);
		
		//controls.set(libcamera::controls::AeLocked, true);
		//controls.set(libcamera::controls::AeExposureMode, libcamera::controls::AeExposureModeEnum::ExposureCustom); //I think it's not programmed, so we can't use it
		
		controls.set(libcamera::controls::ExposureTime, 5000); //20000ms => 50Hz max, TODO: test to see the best value
		//controls.set(libcamera::controls::AfMode, libcamera::controls::AfModeEnum::AfModeAuto);
		controls.set(libcamera::controls::LensPosition, 0);
		//TODO: Handle focus and luminosity correctly
		
		controls.set(libcamera::controls::FrameDurationLimits, libcamera::Span<const std::int64_t, 2>(frameDurationLimits));	
		
		//controls.set(libcamera::controls::AnalogueGain, 16.f); //Significantly decrease perfomance when fullscreen detecting
		
		iteration++;
	}
#endif //ARM
