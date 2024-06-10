import numpy as np
import cv2
from math import pi
import time
import matplotlib.pyplot as plt
import picamera2

camera = picamera2.Picamera2()
cameraConfig = camera.create_video_configuration(main={"format": "RGB888", "size": (2304, 1296)})#(4608,  2592)})
camera.configure(cameraConfig)
camera.set_controls({"ExposureTime": 2000, "ExposureValue": 8.0, "AnalogueGain": 20.0})
camera.start()
print(camera.capture_metadata()["ExposureTime"])
time.sleep(2)

def track(matrix_coefficients, distortion_coefficients, tagId, tagSize):
    sampleNum = []
    posX = []
    posY = []
    posZ = []
    rotX = []
    rotY = []
    rotZ = []
    rot1 = []
    rot2 = []
    rot3 = []
    
    arucoDict = cv2.aruco.Dictionary_get(cv2.aruco.DICT_4X4_100)
    """arucoDetectorParameters = cv2.aruco.DetectorParameters_create()
    #arucoDetectorParameters.useAruco3Detection = False
    arucoDetectorParameters.cornerRefinementMethod = 1
    arucoDetectorParameters.cornerRefinementMinAccuracy = 0.01
    arucoDetectorParameters.cornerRefinementMaxIterations = 100"""
    arucoDetectorParameters = cv2.aruco.DetectorParameters_create()
    arucoDetectorParameters.adaptiveThreshConstant = 7
    arucoDetectorParameters.adaptiveThreshWinSizeMin = 5  # Augmenté pour une meilleure balance
    arucoDetectorParameters.adaptiveThreshWinSizeMax = 23  # Réduit pour minimiser les calculs
    arucoDetectorParameters.adaptiveThreshWinSizeStep = 10  # Augmenté pour réduire les variations
    arucoDetectorParameters.cornerRefinementMaxIterations = 30  # Réduit pour accélérer le raffinement
    arucoDetectorParameters.cornerRefinementMinAccuracy = 0.01  # Augmenté pour permettre un arrêt plus rapide
    arucoDetectorParameters.perspectiveRemoveIgnoredMarginPerCell = 0.13  # Réduit pour moins de calculs
    arucoDetectorParameters.maxErroneousBitsInBorderRate = 0.04  # Reste inchangé
    arucoDetectorParameters.minMarkerPerimeterRate = 0.03  # Augmenté pour ignorer les très petits marqueurs
    arucoDetectorParameters.maxMarkerPerimeterRate = 4.0  # Réduit pour ignorer les grands objets
    arucoDetectorParameters.minOtsuStdDev = 5.0  # Reste inchangé
    arucoDetectorParameters.errorCorrectionRate = 0.6  # Réduit pour diminuer le temps de correction
    
    iteration = 0
    while True:
        
        time0 = time.time()
        
        capturedFrame = camera.capture_array()
        print(capturedFrame.shape)

        if capturedFrame is None:
            print("Video Ended")
            break
            
        frame = cv2.threshold(capturedFrame, 90, 100, cv2.THRESH_BINARY)[1]
        
        time1 = time.time()

        corners, ids, rejectedImgPoints = cv2.aruco.detectMarkers(frame, arucoDict, parameters=arucoDetectorParameters)

        time2 = time.time()

        if np.all(ids is not None):
            for i in range(0, len(ids)):
                rvec, tvec, markerPoints = cv2.aruco.estimatePoseSingleMarkers(corners[i], 100, matrix_coefficients,
                                                                           distortion_coefficients)
                (rvec - tvec).any()  # get rid of that nasty numpy value array error
                
                print("Before:", rvec)
                cv2.aruco.drawDetectedMarkers(frame, corners)  # Draw A square around the markers
                cv2.drawFrameAxes(frame, matrix_coefficients, distortion_coefficients, rvec, tvec, tagSize, 1)  # Draw Axis
                cv2.putText(frame, str(ids[i]),
                    (int(corners[i][0][0][0]), int(corners[i][0][0][1]) - 15), cv2.FONT_HERSHEY_SIMPLEX,
                    1, (200, 0, 200), 2)
                
                if ids[i] == tagId:
                    sampleNum.append(iteration)
                    posX.append(round(tvec.tolist()[0][0][0], 2))
                    posY.append(round(-tvec.tolist()[0][0][1], 2))
                    posZ.append(round(tvec.tolist()[0][0][2], 2))
                    rotX.append(round(rvec.tolist()[0][0][0]*180/pi, 2))
                    rotY.append(round(-rvec.tolist()[0][0][2]*180/pi, 2))
                    rotZ.append(round(rvec.tolist()[0][0][1]*180/pi, 2))
                    rot1.append(rvec.tolist()[0][0][0])
                    rot2.append(rvec.tolist()[0][0][1])
                    rot3.append(rvec.tolist()[0][0][2])
                    
        time3 = time.time() + 0.00000001

        # Display the resulting frame
        cv2.imshow("Camera", frame)
        
        time4 = time.time() + 0.00000001

        # Wait 1 milisecoonds for an interaction. Check the key and do the corresponding job.
        key = cv2.waitKey(1) & 0xFF
        if key == ord('q') or iteration == 100:  # Quit
            break
        
        time5 = time.time()
        print("Execution time and frequency: {}s, {}Hz".format(round(time5 - time0, 4), round(1/(time5 - time0), 2)))
        print("0 => 1: {}s, {}Hz, {}%".format(round(time1 - time0, 4), round(1/(time1 - time0), 2), round((time1 - time0)/(time5 - time0)*100, 2)))
        print("1 => 2: {}s, {}Hz, {}%".format(round(time2 - time1, 4), round(1/(time2 - time1), 2), round((time2 - time1)/(time5 - time0)*100, 2)))
        print("2 => 3: {}s, {}Hz, {}%".format(round(time3 - time2, 4), round(1/(time3 - time2), 2), round((time3 - time2)/(time5 - time0)*100, 2)))
        print("3 => 4: {}s, {}Hz, {}%".format(round(time4 - time3, 4), round(1/(time4 - time3), 2), round((time4 - time3)/(time5 - time0)*100, 2)))
        print("4 => 5: {}s, {}Hz, {}%".format(round(time5 - time4, 4), round(1/(time5 - time4), 2), round((time5 - time4)/(time5 - time0)*100, 2)))
        print("")

        iteration += 1

        #if len(sampleNum) >= 100:
        #    break

    print("Sample number: {}\n\nposX = {}\nposY = {}\nposZ = {}\n\nrotX = {}\nrotY = {}\nrotZ = {}".format(sampleNum, posX, posY, posZ, rotX, rotY, rotZ))

    # When everything done, release the capture
    cv2.destroyAllWindows()

    # Initialise the subplot function using number of rows and columns
    figure, axis = plt.subplots(2, 2)

    axis[0][0].set_title("Position")
    axis[0][0].plot(posX) #.scatter(posX)
    axis[0][0].plot(posY)
    axis[0][0].plot(posZ)
    axis[0][0].legend(['posX', 'posY', 'posZ'], loc='upper right')
    
    axis[0][1].set_title("Rotation")
    axis[0][1].plot(rotX) #.scatter(rotX)
    axis[0][1].plot(rotY)
    axis[0][1].plot(rotZ)
    axis[0][1].legend(['rotX', 'rotY', 'rotZ'], loc='upper right')
    
    axis[1][1].set_title("Rotation")
    axis[1][1].plot(rot1) #.scatter(rotX)
    axis[1][1].plot(rot2)
    axis[1][1].plot(rot3)
    axis[1][1].legend(['rot1', 'rot2', 'rot3'], loc='upper right')
    
    # Combine all the operations and display
    plt.show()

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

[camera_matrix, dist_matrix] = load_coefficients("cameraConfig.yml")
track(camera_matrix, dist_matrix, 3, 70)
