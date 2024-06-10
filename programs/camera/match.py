import numpy as np
import cv2
from math import pi
import picamera2
import socket
import errno

camera = picamera2.Picamera2()
cameraConfig = camera.create_video_configuration(main={"format": "RGB888", "size": (2304, 1296)})#(4608,  2592)})
camera.configure(cameraConfig)
camera.set_controls({"ExposureTime": 2000, "ExposureValue": 8.0, "AnalogueGain": 20.0})
camera.start()
print(camera.capture_metadata()["ExposureTime"])

PRINT_INFOS = False
BLUE_TEAM = 0
YELLOW_TEAM = 1

TAG_LIMITS = [[1, 5], [6, 10]]

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
    
def getNearestValidTag(targetTag, testingTags):
	minDistance = float("inf")
	minDistanceIndex = None
	for i in range(len(testingTags)):
		if not np.array_equal(testingTags[i], np.array([0.0, 0.0, 0.0]).reshape((3, 1))):
			distance = ((targetTag[0]-testingTags[i][0])**2 + (targetTag[1]-testingTags[i][1])**2 + (targetTag[2]-testingTags[i][2])**2)**0.5
			if distance < minDistance:
				minDistance = distance
				minDistanceIndex = i
	return minDistanceIndex

def track(matrix_coefficients, distortion_coefficients, cameraSocket, team):	
	staticTagsRelativePositions = [np.array([0.0, 0.0, 0.0]).reshape((3, 1))]*4
	staticTagsRelativeRotations = [np.array([0.0, 0.0, 0.0]).reshape((3, 1))]*4
	staticTagsAbsolutePositions = [np.array([750.0, 1550.0, 0.0]).reshape((3, 1)), np.array([2250.0, 1550.0, 0.0]).reshape((3, 1)), np.array([750.0, 450.0, 0.0]).reshape((3, 1)), np.array([2250.0, 450.0, 0.0]).reshape((3, 1))]
	staticTagsAbsoluteRotations = [np.array([0.0, 0.0, 0.0]).reshape((3, 1))]*4
	ownRobotTagRelativePosition = np.array([0.0, 0.0, 0.0]).reshape((3, 1))
	ownRobotTagRelativeRotation = np.array([0.0, 0.0, 0.0]).reshape((3, 1))
	ownRobotTagAbsolutePosition = np.array([0.0, 0.0, 0.0]).reshape((3, 1))
	ownRobotTagAbsoluteRotation = np.array([0.0, 0.0, 0.0]).reshape((3, 1))
	advRobotTagRelativePosition = np.array([0.0, 0.0, 0.0]).reshape((3, 1))
	advRobotTagRelativeRotation = np.array([0.0, 0.0, 0.0]).reshape((3, 1))
	advRobotTagAbsolutePosition = np.array([0.0, 0.0, 0.0]).reshape((3, 1))
	advRobotTagAbsoluteRotation = np.array([0.0, 0.0, 0.0]).reshape((3, 1))
	
	arucoDict = cv2.aruco.Dictionary_get(cv2.aruco.DICT_4X4_100)
	arucoDetectorParameters = cv2.aruco.DetectorParameters_create()
	#arucoDetectorParameters.useAruco3Detection = False
	arucoDetectorParameters.cornerRefinementMethod = 1
	arucoDetectorParameters.cornerRefinementMinAccuracy = 0.01
	arucoDetectorParameters.cornerRefinementMaxIterations = 100
	
	iteration = 0
	while True:
		frame = camera.capture_array()
		#frame = cv2.rotate(frame, cv2.ROTATE_180) not needed as the computed robot position is absolute and not relative to the camera
		
		if frame is None:
			print("Video Ended")
			break
			
		#gray = cv2.cvtColor(frame, cv2.COLOR_RGB2GRAY) not needed as detectMarkers seems to run at the same speed without

		corners, ids, rejectedImgPoints = cv2.aruco.detectMarkers(frame, arucoDict, parameters=arucoDetectorParameters)
		
		if np.all(ids is not None):
			ids = [tagId[0] for tagId in ids]
			getNewOwnRobotPose = False
			getNewAdvRobotPose = False
			for i, computingTagId in enumerate(ids):
				rvec, tvec, markerPoints = cv2.aruco.estimatePoseSingleMarkers(corners[i], getTagSize(computingTagId), matrix_coefficients, distortion_coefficients)
				(rvec - tvec).any()  # get rid of that nasty numpy value array error
				
				if rvec.tolist()[0][0][1] > 0:
					rvec *= -1
				
				cv2.aruco.drawDetectedMarkers(frame, corners)  # Draw A square around the markers
				cv2.drawFrameAxes(frame, matrix_coefficients, distortion_coefficients, rvec, tvec, getTagSize(computingTagId)/4, 1)  # Draw Axis
				cv2.putText(frame, str(ids[i]), (int(corners[i][0][0][0]), int(corners[i][0][0][1]) - 15), cv2.FONT_HERSHEY_SIMPLEX, 1, (200, 0, 200), 2)
				
				val = computingTagId-20
				if 20 <= computingTagId <= 23 and (np.array_equal(staticTagsRelativePositions[val], np.array([0.0, 0.0, 0.0]).reshape((3, 1))) or iteration < 5):
					staticTagsRelativePositions[computingTagId-20] = np.array(tvec.tolist()[0][0]).reshape((3, 1))
					staticTagsRelativeRotations[computingTagId-20] = np.array(rvec.tolist()[0][0]).reshape((3, 1))
				elif TAG_LIMITS[team][0] <= computingTagId <= TAG_LIMITS[team][1]:
					ownRobotTagRelativePosition = np.array(tvec.tolist()[0][0]).reshape((3, 1))
					ownRobotTagRelativeRotation = np.array(rvec.tolist()[0][0]).reshape((3, 1))
					getNewOwnRobotPose = True
				elif TAG_LIMITS[1-team][0] <= computingTagId <= TAG_LIMITS[1-team][1]:
					advRobotTagRelativePosition = np.array(tvec.tolist()[0][0]).reshape((3, 1))
					advRobotTagRelativeRotation = np.array(rvec.tolist()[0][0]).reshape((3, 1))
					getNewAdvRobotPose = True
	
			nearestValidTagIndex = getNearestValidTag(ownRobotTagRelativePosition, staticTagsRelativePositions)
			ownRobotComposedPose = cv2.composeRT(*relativePosition(ownRobotTagRelativeRotation, ownRobotTagRelativePosition, staticTagsRelativeRotations[nearestValidTagIndex], staticTagsRelativePositions[nearestValidTagIndex]), staticTagsAbsoluteRotations[nearestValidTagIndex], staticTagsAbsolutePositions[nearestValidTagIndex])
			ownRobotTagAbsoluteRotation, ownRobotTagAbsolutePosition = (ownRobotComposedPose[0] + np.array([0.0, 0.0, 1.571]).reshape((3, 1)))%6.283, ownRobotComposedPose[1]
			
			nearestValidTagIndex = getNearestValidTag(advRobotTagRelativePosition, staticTagsRelativePositions)
			advRobotComposedPose = cv2.composeRT(*relativePosition(advRobotTagRelativeRotation, advRobotTagRelativePosition, staticTagsRelativeRotations[nearestValidTagIndex], staticTagsRelativePositions[nearestValidTagIndex]), staticTagsAbsoluteRotations[nearestValidTagIndex], staticTagsAbsolutePositions[nearestValidTagIndex])
			advRobotTagAbsoluteRotation, advRobotTagAbsolutePosition = (advRobotComposedPose[0] + np.array([0.0, 0.0, 1.571]).reshape((3, 1)))%6.283, advRobotComposedPose[1]
			
			if PRINT_INFOS:
				print()
				for i in range(4):
					print(f"Static tag {20+i} relative pos: X={staticTagsRelativePositions[i][0]}, Y={staticTagsRelativePositions[i][1]}, Z={staticTagsRelativePositions[i][2]}")
					print(f"Static tag {20+i} relative rot: X={staticTagsRelativeRotations[i][0]}, Y={staticTagsRelativeRotations[i][1]}, Z={staticTagsRelativeRotations[i][2]}")
					print(f"Static tag {20+i} absolute pos: X={staticTagsAbsolutePositions[i][0]}, Y={staticTagsAbsolutePositions[i][1]}, Z={staticTagsAbsolutePositions[i][2]}")
			
			if getNewOwnRobotPose:
				if PRINT_INFOS:
					print(f"Own robot tag relative pos: X={ownRobotTagRelativePosition[0]}, Y={ownRobotTagRelativePosition[1]}, Z={ownRobotTagRelativePosition[2]}")
					print(f"Own robot tag absolute pos: X={ownRobotTagAbsolutePosition[0]}, Y={ownRobotTagAbsolutePosition[1]}, Z={ownRobotTagAbsolutePosition[2]}")
				cameraSocket.send(bytes(f"A {ownRobotTagAbsolutePosition[0][0]} {ownRobotTagAbsolutePosition[1][0]} {ownRobotTagAbsoluteRotation[2][0]}", "ascii"))
			
			if getNewAdvRobotPose:
				if PRINT_INFOS:
					print(f"Adv robot tag relative pos: X={advRobotTagRelativePosition[0]}, Y={advRobotTagRelativePosition[1]}, Z={advRobotTagRelativePosition[2]}")
					print(f"Adv robot tag absolute pos: X={advRobotTagAbsolutePosition[0]}, Y={advRobotTagAbsolutePosition[1]}, Z={advRobotTagAbsolutePosition[2]}")
				cameraSocket.send(bytes(f"B {advRobotTagAbsolutePosition[0][0]} {advRobotTagAbsolutePosition[1][0]} {advRobotTagAbsoluteRotation[2][0]}", "ascii"))
			
	
		# Display the resulting frame
		#cv2.imshow("Camera", frame)
	
		# Wait 1 milisecoonds for an interaction. Check the key and do the corresponding job.
		key = cv2.waitKey(1) & 0xFF
		if key == ord('q'):  # Quit
			break
			
		iteration += 1

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
	
	host = "192.168.219.245" #"rpiTomIUT"
	port = 8910
	cameraSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	
	while 1:
		try:
			print(f"Trying to connect {host}:{port} ...")
			cameraSocket.connect((host, port))
			print("Connected")
			track(camera_matrix, dist_matrix, cameraSocket, YELLOW_TEAM)
		except socket.error as e:
			print(e)
			if e.errno == 106:
				print("Disconnect !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
				cameraSocket.close()
			elif e.errno == 9:
				cameraSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		except Exception as e:
			print(e)
			
	cameraSocket.close()
	
	cameraSocket.close()
	# When everything done, release the capture
	cv2.destroyAllWindows()
