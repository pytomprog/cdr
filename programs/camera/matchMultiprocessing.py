import numpy as np
import cv2
from math import pi
import picamera2
import socket
import multiprocessing
import time

camera = picamera2.Picamera2()
cameraConfig = camera.create_still_configuration(main={"format": "RGB888", "size": (4608,  2592)})
camera.configure(cameraConfig)
camera.set_controls({"ExposureTime": 5000, "ExposureValue": 8.0, "AnalogueGain": 20.0})
camera.start()
print(camera.capture_metadata()["ExposureTime"])


def getTagSize(tagId):
    if 1 <= tagId <= 10:
        return 70
    elif 20 <= tagId <= 23:
        return 100
    else:
        return 50
    
def inversePerspective(rvec, tvec):
    R, _ = cv2.Rodrigues(rvec)
    R = np.matrix(R).T
    invTvec = np.dot(-R, np.matrix(tvec))
    invRvec, _ = cv2.Rodrigues(R)
    return invRvec, invTvec


def relativePosition(rvec1, tvec1, rvec2, tvec2):
    rvec1, tvec1 = rvec1.reshape((3, 1)), tvec1.reshape((3, 1))
    rvec2, tvec2 = rvec2.reshape((3, 1)), tvec2.reshape((3, 1))

    # Inverse the second marker, the right one in the image
    invRvec, invTvec = inversePerspective(rvec2, tvec2)

    orgRvec, orgTvec = inversePerspective(invRvec, invTvec)
    # print("rvec: ", rvec2, "tvec: ", tvec2, "\n and \n", orgRvec, orgTvec)
    
    composedPose = cv2.composeRT(rvec1, tvec1, invRvec, invTvec)
    composedRvec, composedTvec = composedPose[0], composedPose[1]

    composedRvec = composedRvec.reshape((3, 1))
    composedTvec = composedTvec.reshape((3, 1))
    return composedRvec, composedTvec

def cameraProcessMain(frame, cameraImageIndex, processedImageIndex):
	while True:
		print("Getting new image ...")
		frame.value = camera.capture_array()
		#frame = cv2.rotate(frame, cv2.ROTATE_180) not needed as the computed robot position is absolute and not relative to the camera
		
		if frame.value is None:
			print("Video Ended")
			break
			
		#gray = cv2.cvtColor(frame, cv2.COLOR_RGB2GRAY) not needed as detectMarkers seems to run at the same speed without
		
		cameraImageIndex.value += 1
		print(f"Camera process: Camera image index, {cameraImageIndex.value}, processed image index: {processedImageIndex.value}") # processedImageIndex.value is 1 under cameraImageIndex.value because camera frame is new

def computingProcessMain(frame, cameraImageIndex, processedImageIndex, matrix_coefficients, distortion_coefficients, cameraSocket):	
	staticTagRelativePosition = np.array([0.0, 0.0, 0.0]).reshape((3, 1))
	staticTagRelativeRotation = np.array([0.0, 0.0, 0.0]).reshape((3, 1))
	staticTagAbsolutePosition = np.array([750.0, 450.0, 0.0]).reshape((3, 1))
	staticTagAbsoluteRotation = np.array([0.0, 0.0, 0.0]).reshape((3, 1))
	robotTagRelativePosition = np.array([0.0, 0.0, 0.0]).reshape((3, 1))
	robotTagRelativeRotation = np.array([0.0, 0.0, 0.0]).reshape((3, 1))
	robotTagAbsolutePosition = np.array([0.0, 0.0, 0.0]).reshape((3, 1))
	robotTagAbsoluteRotation = np.array([0.0, 0.0, 0.0]).reshape((3, 1))
	
	aruco_dict = cv2.aruco.Dictionary_get(cv2.aruco.DICT_4X4_100)

	while True:
		#print(f"Computing process: Camera image index, {cameraImageIndex.value}, processed image index: {processedImageIndex.value}")
		if processedImageIndex.value < cameraImageIndex.value:
			print("Processing image ...")
			corners, ids, rejected_img_points = cv2.aruco.detectMarkers(frame.value, aruco_dict)
			
			drawingFrame = frame.value
	
			if np.all(ids is not None):
				for i in range(0, len(ids)):
					rvec, tvec, markerPoints = cv2.aruco.estimatePoseSingleMarkers(corners[i], getTagSize(ids[i]), matrix_coefficients, distortion_coefficients)
					(rvec - tvec).any()  # get rid of that nasty numpy value array error
					cv2.aruco.drawDetectedMarkers(drawingFrame, corners)  # Draw A square around the markers
					cv2.drawFrameAxes(drawingFrame, matrix_coefficients, distortion_coefficients, rvec, tvec, getTagSize(ids[i])/4, 1)  # Draw Axis
					cv2.putText(drawingFrame, str(ids[i]), (int(corners[i][0][0][0]), int(corners[i][0][0][1]) - 15), cv2.FONT_HERSHEY_SIMPLEX, 1, (200, 0, 200), 2)
					
					if ids[i] == 22:
						staticTagRelativePosition = np.array(tvec.tolist()[0][0]).reshape((3, 1))
						staticTagRelativeRotation = np.array(rvec.tolist()[0][0]).reshape((3, 1))
					elif ids[i] == 3:
						robotTagRelativePosition = np.array(tvec.tolist()[0][0]).reshape((3, 1))
						robotTagRelativeRotation = np.array(rvec.tolist()[0][0]).reshape((3, 1))
	
				composedPose = cv2.composeRT(*relativePosition(robotTagRelativeRotation, robotTagRelativePosition, staticTagRelativeRotation, staticTagRelativePosition), staticTagAbsoluteRotation, staticTagAbsolutePosition)
				robotTagAbsoluteRotation, robotTagAbsolutePosition = composedPose[0], composedPose[1]
				
				print()
				print(f"Static tag relative pos: X={staticTagRelativePosition[0]}, Y={staticTagRelativePosition[1]}, Z={staticTagRelativePosition[2]}")
				print(f"Static tag absolute pos: X={staticTagAbsolutePosition[0]}, Y={staticTagAbsolutePosition[1]}, Z={staticTagAbsolutePosition[2]}")
				print(f"Robot tag relative pos: X={robotTagRelativePosition[0]}, Y={robotTagRelativePosition[1]}, Z={robotTagRelativePosition[2]}")
				print(f"Robot tag absolute pos: X={robotTagAbsolutePosition[0]}, Y={robotTagAbsolutePosition[1]}, Z={robotTagAbsolutePosition[2]}")
				
				cameraSocket.send(bytes(f"A {robotTagAbsolutePosition[0][0]} {robotTagAbsolutePosition[1][0]} {robotTagAbsoluteRotation[2][0]}", "ascii"))
	
			# Display the resulting frame
			#cv2.imshow("Camera", drawingFrame)
	
			# Wait 1 milisecoonds for an interaction. Check the key and do the corresponding job.
			key = cv2.waitKey(1) & 0xFF
			if key == ord('q'):  # Quit
				break
	
			processedImageIndex.value += 1
		else:
			pass
			#time.sleep(1)
	
	# When everything done, release the capture
	cv2.destroyAllWindows()

def load_coefficients(path):
    """ Loads camera matrix and distortion coefficients. """
    # FILE_STORAGE_READ
    cv_file = cv2.FileStorage(path, cv2.FILE_STORAGE_READ)

    # note we also have to specify the type to retrieve other wise we only get a
    # FileNode object back instead of a matrix
    camera_matrix = cv_file.getNode("K").mat()
    dist_matrix = cv_file.getNode("D").mat()

    cv_file.release()
    return [camera_matrix, dist_matrix]

if __name__ == "__main__":
	[camera_matrix, dist_matrix] = load_coefficients("cameraConfig.yml")
	
	host = "192.168.193.245" #"rpiTomIUT"
	port = 8080
	cameraSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	cameraSocket.connect((host, port))
	
	manager = multiprocessing.Manager()
	
	frame = manager.Value("frame", None)
	cameraImageIndex = manager.Value("cameraImageIndex", 0)
	processedImageIndex = manager.Value("processedImageIndex", 0)
	
	computingProcess = multiprocessing.Process(target=computingProcessMain, args=(frame, cameraImageIndex, processedImageIndex, camera_matrix, dist_matrix, cameraSocket,))
	computingProcess.start()
	cameraProcessMain(frame, cameraImageIndex, processedImageIndex)
	
	cameraSocket.close()
