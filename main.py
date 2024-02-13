import threading
import time

import cv2
import imutils
import numpy as np
import requests


COLOR_BUCKET_HSV = {
    "black": [[180, 255, 30], [0, 0, 0]],
    "white": [[180, 30, 255], [0, 0, 200]],
    "red": [[10, 255, 255], [0, 100, 100]],
    "green": [[80, 255, 255], [40, 40, 40]],
    "blue": [[130, 255, 255], [90, 50, 50]],
    "yellow": [[30, 255, 255], [20, 100, 100]],
    "purple": [[160, 255, 255], [120, 50, 50]],
    "orange": [[30, 255, 255], [0, 100, 100]],
    "gray": [[180, 30, 150], [0, 0, 0]],
}

COLOR_OPTIONS = {"1": "black", "2": "white", "3": "red", "4": "green",
                 "5": "blue", "6": "yellow", "7": "purple", "8": "orange", "9": "gray", "10": "custom"}

UP_RECT = ((410, 10), (520, 120))
DOWN_RECT = ((410, 230), (520, 340))
MID_RECT = ((410, 120), (520, 230))
LEFT_RECT = ((300, 120), (410, 230))
RIGHT_RECT = ((520, 120), (630, 230))


def input2range(data: str) -> np.array:
    data = data.split(",")
    return np.array([int(x.strip()) for x in data])


def point_in_rectangle(rect, pt):
    x1, y1 = rect[0]
    x2, y2 = rect[1]
    p1, p2 = pt

    if x1 < p1 < x2 and y1 < p2 < y2:
        return True
    return False


def draw_rectangles(img: np.array, active_rect: str = None) -> np.array:
    img = cv2.rectangle(img, UP_RECT[0], UP_RECT[1], (255, 0, 0), 3)
    img = cv2.rectangle(img, DOWN_RECT[0], DOWN_RECT[1], (255, 0, 0), 3)
    img = cv2.rectangle(img, MID_RECT[0], MID_RECT[1], (255, 0, 0), 3)
    img = cv2.rectangle(img, LEFT_RECT[0], LEFT_RECT[1], (255, 0, 0), 3)
    img = cv2.rectangle(img, RIGHT_RECT[0], RIGHT_RECT[1], (255, 0, 0), 3)

    if active_rect == 'u':
        img = cv2.rectangle(img, UP_RECT[0], UP_RECT[1], (0, 255, 0), 3)
    elif active_rect == 'd':
        img = cv2.rectangle(img, DOWN_RECT[0], DOWN_RECT[1], (0, 255, 0), 3)
    elif active_rect == 'm':
        img = cv2.rectangle(img, MID_RECT[0], MID_RECT[1], (0, 255, 0), 3)
    elif active_rect == 'l':
        img = cv2.rectangle(img, LEFT_RECT[0], LEFT_RECT[1], (0, 255, 0), 3)
    elif active_rect == 'r':
        img = cv2.rectangle(img, RIGHT_RECT[0], RIGHT_RECT[1], (0, 255, 0), 3)
    return img


def fetch_url(url):
    try:
        response = requests.get(url)
        print(f"URL: {url} - Status Code: {response.status_code}")
    except Exception:
        print("Error: Couldn't connect to server")


print("*"*80)
print("While the app is loading")
print("Visit https://www.instagram.com/protionixtech/")
print("Tag us with @protionixtech")
print("*"*80)
time.sleep(3)

print("\n!! Make sure the object should be round !!\n")

obj_color = None
while obj_color is None:
    print("Choose object color")
    for k, v in COLOR_OPTIONS.items():
        print(k, " -- ", v)
    obj_color = input(">> ")

    obj_color = COLOR_OPTIONS.get(obj_color, None)
    if obj_color is not None:
        break
print(f">> {obj_color} selected")

if obj_color != "custom":
    higher_range = np.array(COLOR_BUCKET_HSV[obj_color][0])
    lower_range = np.array(COLOR_BUCKET_HSV[obj_color][1])
else:
    higher_range = input(">> Higher range (comma separated): ")
    higher_range = input2range(higher_range)
    lower_range = input(">> Lower range (comma separated): ")
    lower_range = input2range(lower_range)

print("Higher range: ", higher_range)
print("Lower range: ", lower_range)

last_status = 's'

cam = cv2.VideoCapture(0)
while True:
    check, frame = cam.read()
    if not check:
        print("Could not read from camera")
        break

    frame = cv2.flip(frame, 1)
    hsv_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    mask = cv2.inRange(hsv_frame[10:340, 300:630], lower_range, higher_range)
    cv2.imshow('mask', mask)

    contours = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    contours = imutils.grab_contours(contours)

    status = 's'
    active_rect = None
    try:
        max_area = max(list(map(lambda c: cv2.contourArea(c), contours)))

        cx, cy = None, None
        for c in contours:
            area = cv2.contourArea(c)
            if area != max_area:
                continue
            moments = cv2.moments(c)
            if moments['m00'] == 0.0:
                continue
            cx = int(moments['m10']/moments['m00']) + 300
            cy = int(moments['m01']/moments['m00']) + 10
            cv2.circle(frame, (cx, cy), 3, (255, 255, 255), 3)

        if point_in_rectangle(UP_RECT, (cx, cy)):
            status = 'f'
            active_rect = 'u'
        elif point_in_rectangle(DOWN_RECT, (cx, cy)):
            status = 'b'
            active_rect = 'd'
        elif point_in_rectangle(MID_RECT, (cx, cy)):
            status = 's'
            active_rect = 'm'
        elif point_in_rectangle(LEFT_RECT, (cx, cy)):
            status = 'l'
            active_rect = 'l'
        elif point_in_rectangle(RIGHT_RECT, (cx, cy)):
            status = 'r'
            active_rect = 'r'

    except Exception as err:
        print("Error: ", err)

    frame = draw_rectangles(frame, active_rect)
    cv2.imshow('processed frame', frame)

    if last_status != status:
        last_status = status
        url = f"http://192.168.1.1/?movement={status}"
        thread = threading.Thread(target=fetch_url, args=(url,), daemon=True)
        thread.start()

    key = cv2.waitKey(33)
    if key == 27:
        break

cv2.destroyAllWindows()
cam.release()


