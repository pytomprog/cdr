import glob
import picamera2
import os
import cv2

scriptPath = "/".join(os.path.realpath(__file__).split("/")[:-1])

camera = picamera2.Picamera2()
cameraConfig = camera.create_still_configuration(main={"format": "RGB888", "size": (2304, 1296)})#(4608,  2592)})
camera.configure(cameraConfig)
camera.set_controls({"ExposureTime": 2000, "ExposureValue": 8.0, "AnalogueGain": 20.0})
camera.start()
print(camera.capture_metadata()["ExposureTime"])


while True:
    image = camera.capture_array()
    #image = cv2.rotate(image, cv2.ROTATE_180)

    cv2.imshow("Camera", image)

    key = cv2.waitKey(1) & 0xFF
    if key == ord('q'):
        #quit
        break
    elif key == ord('s'):
        #save current image
        pngCounter = len(glob.glob1(scriptPath + "/calibrationImages/", "image*.png"))
        print(pngCounter)
        cv2.imwrite(scriptPath + "/calibrationImages/image{}.png".format(pngCounter+1) , image)


cv2.destroyWindow("Camera")
