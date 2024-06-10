from io import BytesIO
from time import sleep
import picamera2
import cv2

# Create an in-memory stream
my_stream = BytesIO()
camera = picamera2.Picamera2()
cameraConfig = camera.create_still_configuration(main={"format": "RGB888"})
camera.configure(cameraConfig)
camera.start()

while True:
	image = camera.capture_array()
	image = cv2.rotate(image, cv2.ROTATE_180)
		
	arucoDict = cv2.aruco.Dictionary_get(cv2.aruco.DICT_4X4_100)
	arucoParams = cv2.aruco.DetectorParameters_create()
	(corners, ids, rejected) = cv2.aruco.detectMarkers(image, arucoDict, parameters=arucoParams)
	image_output = cv2.aruco.drawDetectedMarkers(image, corners, ids)
		
	cv2.imshow("Camera", image_output)
	
	#image shape
	#print(image.shape)
	#h, w, c = image.shape
	#print('width: ', w)
	#print('height: ', h)
	#print('channel: ', c)
	
	if cv2.waitKey(1)==ord('q'):
		break

cv2.destroyAllWindows()
