from ultralytics import YOLO
import cv2
import torch

d_model = YOLO('runs/detect/train/weights/best.pt')
y_model = YOLO('yolov8n.pt')

cap = cv2.VideoCapture(1)

if cap.isOpened():
    print('width: {}, height : {}'.format(cap.get(3), cap.get(4)))

while True:
    ret, frame = cap.read()

    if ret:
        frame = cv2.flip(frame, 1)

        y_result = y_model([frame])[0]
        y_res_plot = y_result.plot()
        y_data = y_result.boxes.data
        is_person = torch.any(y_data[:, -1] == 0).item()

        d_result = d_model([frame])[0]
        d_res_plot = d_result.plot()
        d_data = d_result.boxes.data  # tensor([[x,y,x,y,conf,cls]]) torch.float32

        filtered_data = d_data[d_data[:, -1] == 0]

        cv2.imshow("res", y_res_plot)

        k = cv2.waitKey(1) & 0xFF
        if k == 27:
            break
    else:
        print('error')
        break

cap.release()
cv2.destroyAllWindows()
