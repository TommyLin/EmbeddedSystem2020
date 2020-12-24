import time
t1 = time.time()
import face_recognition
t2 = time.time()
print ("import face_recognition time = %f" % (t2 - t1))

t1 = time.time()
import cv2
t2 = time.time()
print ("import cv2 time = %f" % (t2 - t1))

t1 = time.time()
import numpy as np
t2 = time.time()
print ("import numpy time = %f" % (t2 - t1))

# This is a demo of running face recognition on live video from your webcam. It's a little more complicated than the
# other example, but it includes some basic performance tweaks to make things run a lot faster:
#   1. Process each video frame at 1/4 resolution (though still display it at full resolution)
#   2. Only detect faces in every other frame of video.

# PLEASE NOTE: This example requires OpenCV (the `cv2` library) to be installed only to read from your webcam.
# OpenCV is *not* required to use the face_recognition library. It's only required if you want to run this
# specific demo. If you have trouble installing it, try any of the other demos that don't require it instead.

# Get a reference to webcam #0 (the default one)
t1 = time.time()
video_capture = cv2.VideoCapture(0)
t2 = time.time()
print ("video_capture time = %f" % (t2 - t1))

# Load a sample picture and learn how to recognize it.
t1 = time.time()
obama_image = face_recognition.load_image_file("./know/obama.jpg")
t2 = time.time()
print ("face_recognition.load_image_file obama time = %f" % (t2 - t1))

t1 = time.time()
obama_face_encoding = face_recognition.face_encodings(obama_image)[0]
t2 = time.time()
print ("iface_recognition.face_encodings obama_image time = %f" % (t2 - t1))

print (type(obama_face_encoding))

# Load a second sample picture and learn how to recognize it.
t1 = time.time()
biden_image = face_recognition.load_image_file("./know/biden.jpg")
t2 = time.time()
print ("face_recognition.load_image_file biden time = %f" % (t2 - t1))

t1 = time.time()
biden_face_encoding = face_recognition.face_encodings(biden_image)[0]
t2 = time.time()
print ("face_recognition.face_encodings biden_image time = %f" % (t2 - t1))

# Load a third sample picture and learn how to recognize it.
t1 = time.time()
suzen_image = face_recognition.load_image_file("./know/suzen.jpg")
t2 = time.time()
print ("face_recognition.load_image_file suzen time = %f" % (t2 - t1))

t1 = time.time()
suzen_face_encoding = face_recognition.face_encodings(suzen_image)[0]
t2 = time.time()
print ("face_recognition.face_encodings suzen_image time = %f" % (t2 - t1))

# Load a forth sample picture and learn how to recognize it.
t1 = time.time()
zenna_image = face_recognition.load_image_file("./know/zenna.jpg")
t2 = time.time()
print ("face_recognition.load_image_file zenna time = %f" % (t2 - t1))

t1 = time.time()
zenna_face_encoding = face_recognition.face_encodings(zenna_image)[0]
t2 = time.time()
print ("face_recognition.face_encodings zenna_image time = %f" % (t2 - t1))

# Load a fifth sample picture and learn how to recognize it.
# t1 = time.time()
# yen_image = face_recognition.load_image_file("./know/ChesterYen.jpg")
# t2 = time.time()
# print ("face_recognition.load_image_file ChesterYen time = %f" % (t2 - t1))

# t1 = time.time()
# yen_face_encoding = face_recognition.face_encodings(yen_image)[0]
# t2 = time.time()
# print ("face_recognition.face_encodings yen_image time = %f" % (t2 - t1))

# Create arrays of known face encodings and their names
known_face_encodings = [
    obama_face_encoding,
    biden_face_encoding,
    suzen_face_encoding,
    zenna_face_encoding
#    yen_face_encoding
]
known_face_names = [
    "Barack Obama",
    "Joe Biden",
    "Suzen Chuang",
    "Zenna Tsai"
#    "Chester Yen"
]

# Initialize some variables
face_locations = []
face_encodings = []
face_names = []
process_this_frame = 1

while True:
    # Grab a single frame of video
    t1 = time.time()
    ret, frame = video_capture.read()
    t2 = time.time()
    print ("video_capture.read time = %f" % (t2 - t1))

    # Resize frame of video to 1/4 size for faster face recognition processing
    t1 = time.time()
    small_frame = cv2.resize(frame, (0, 0), fx=0.25, fy=0.25)
    t2 = time.time()
    print ("resize time = %f" % (t2 - t1))

    # Convert the image from BGR color (which OpenCV uses) to RGB color (which face_recognition uses)
    rgb_small_frame = small_frame[:, :, ::-1]

    # Only process every other frame of video to save time
    if (process_this_frame == 1):
        # Find all the faces and face encodings in the current frame of video
        t1 = time.time()
        face_locations = face_recognition.face_locations(rgb_small_frame)
        t2 = time.time()
        print ("face_locations time = %f" % (t2 - t1))

        t1 = time.time()
        face_encodings = face_recognition.face_encodings(rgb_small_frame, face_locations)
        t2 = time.time()
        print ("face_encodings time = %f" % (t2 - t1))

        face_names = []
        for face_encoding in face_encodings:
            # See if the face is a match for the known face(s)
            t1 = time.time()
            matches = face_recognition.compare_faces(known_face_encodings, face_encoding)
            t2 = time.time()
            print ("compare_faces time = %f" % (t2 - t1))

            name = "Unknown"

            # # If a match was found in known_face_encodings, just use the first one.
            # if True in matches:
            #     first_match_index = matches.index(True)
            #     name = known_face_names[first_match_index]

            # Or instead, use the known face with the smallest distance to the new face
            t1 = time.time()
            face_distances = face_recognition.face_distance(known_face_encodings, face_encoding)
            t2 = time.time()
            print ("face_distance time = %f" % (t2 - t1))

            t1 = time.time()
            best_match_index = np.argmin(face_distances)
            t2 = time.time()
            print ("argmin time = %f" % (t2 - t1))

            if matches[best_match_index]:
                name = known_face_names[best_match_index]

            face_names.append(name)

    process_this_frame = (process_this_frame + 1 ) % 4


    # Display the results
    for (top, right, bottom, left), name in zip(face_locations, face_names):
        # Scale back up face locations since the frame we detected in was scaled to 1/4 size
        top *= 4
        right *= 4
        bottom *= 4
        left *= 4

        # Draw a box around the face
        t1 = time.time()
        cv2.rectangle(frame, (left, top), (right, bottom), (0, 0, 255), 2)
        t2 = time.time()
        print ("Draw a box rectangle time = %f" % (t2 - t1))

        # Draw a label with a name below the face
        t1 = time.time()
        cv2.rectangle(frame, (left, bottom - 35), (right, bottom), (0, 0, 255), -1)
        t2 = time.time()
        print ("Draw a label rectangle time = %f" % (t2 - t1))


        font = cv2.FONT_HERSHEY_DUPLEX
        cv2.putText(frame, name, (left + 6, bottom - 6), font, 1.0, (255, 255, 255), 1)

    # Display the resulting image
    cv2.imshow('Video', frame)

    # Hit 'q' on the keyboard to quit!
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release handle to the webcam
video_capture.release()
cv2.destroyAllWindows()
