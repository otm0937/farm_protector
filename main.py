import time

import cv2
from ultralytics import YOLO

model = YOLO('runs/detect/train/weights/best.pt')

cap = cv2.VideoCapture(1)

if cap.isOpened():
    print('width: {}, height : {}'.format(cap.get(3), cap.get(4)))

prev_time = time.time()
cnt = 0
avg = 1.0
max_fps = 0
min_fps = 100000

while True:
    ret, frame = cap.read()

    if ret:
        frame = cv2.flip(frame, 1)

        inputs = [frame]
        results = model(inputs)
        res_plotted = results[0].plot()
        now_time = time.time()

        dly = now_time - prev_time
        fps = (1 / dly)
        avg = (avg * cnt + fps) / (cnt + 1)
        cnt += 1
        prev_time = now_time

        if fps > max_fps:
            max_fps = fps
        if fps < min_fps:
            min_fps = fps

        cv2.putText(res_plotted, "cnt: %d" % cnt, (5, 50), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (100, 255, 0), 1)
        cv2.putText(res_plotted, "dly: %.4f" % dly, (5, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (100, 255, 0), 1)
        cv2.putText(res_plotted, "fps: %.2f" % fps, (5, 70), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (100, 255, 0), 1)
        cv2.putText(res_plotted, "avg: %.2f" % avg, (5, 90), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (100, 255, 0), 1)
        cv2.putText(res_plotted, "max: %.2f" % max_fps, (5, 110), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (100, 255, 0), 1)
        cv2.putText(res_plotted, "min: %.2f" % min_fps, (5, 130), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (100, 255, 0), 1)

        cv2.imshow("res", res_plotted)

        k = cv2.waitKey(1) & 0xFF
        if k == 27:
            break
    else:
        print('error')
        break


cap.release()
cv2.destroyAllWindows()