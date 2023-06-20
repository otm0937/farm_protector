from ultralytics import YOLO
import cv2
import torch
import serial
import time


def transform_data(data):
    x1, y1, x2, y2, conf, cls = torch.split(data, 1, dim=1)
    x = (x1 + x2) / 2
    y = (y1 + y2) / 2
    transformed_data = torch.cat([x, y, cls], dim=1)
    return transformed_data.to(torch.int)


def get_min_x_data(data):
    min_x_index = torch.argmin(data[:, 0])
    min_x_data = data[min_x_index]
    return min_x_data


#d_model = YOLO('runs/detect/train/weights/best.pt')
d_model = YOLO('model/best.pt')
y_model = YOLO('yolov8n.pt')

ser = serial.Serial('COM10', 115200)
print(ser.name)

cap = cv2.VideoCapture(1, cv2.CAP_DSHOW)

if cap.isOpened():
    print('width: {}, height : {}'.format(cap.get(3), cap.get(4)))

prev_time = time.time()  # init time
cnt = 0
avg = 1.0
max_fps = 0
min_fps = 100000

while True:
    ret, frame = cap.read()

    if ret:
        frame = cv2.flip(frame, 0)

        y_result = y_model([frame])[0]
        y_res_plot = y_result.plot()
        y_data = y_result.boxes.data
        is_person = torch.any(y_data[:, -1] == 0).item()

        result = '@1' if is_person else '@0'

        d_result = d_model.predict(
            source=[frame],
            conf=0.5
        )[0]
        d_res_plot = d_result.plot()
        d_data = d_result.boxes.data  # tensor([[x1,y1,x2,y2,conf,cls]]) torch.float32

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

        res_plot = cv2.hconcat([y_res_plot, d_res_plot])
        cv2.putText(res_plot, "cnt: %d" % cnt, (5, 50), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (100, 255, 0), 1)
        cv2.putText(res_plot, "dly: %.4f" % dly, (5, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (100, 255, 0), 1)
        cv2.putText(res_plot, "fps: %.2f" % fps, (5, 70), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (100, 255, 0), 1)
        cv2.putText(res_plot, "avg: %.2f" % avg, (5, 90), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (100, 255, 0), 1)
        cv2.putText(res_plot, "max: %.2f" % max_fps, (5, 110), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (100, 255, 0), 1)
        cv2.putText(res_plot, "min: %.2f" % min_fps, (5, 130), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (100, 255, 0), 1)
        cv2.putText(res_plot, str(is_person), (5, 150), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (100, 255, 0), 1)
        cv2.imshow("result", res_plot)

        if d_data.numel() == 0:
            result = result + ',' + '0,0,0,0'
        else:
            result = result + ',' + '1'

            data = transform_data(d_data)  # tensor([[x,y,cls]])

            datum = get_min_x_data(data)

            result = result + ',' + f"{datum[0]},{datum[1]},{datum[2]}\n"

        print(result)

        try:
            ser.write(result.encode())
        except:
            print('error')
            ser.close()
            ser = serial.Serial('COM10')
            print(ser.name)

        k = cv2.waitKey(1) & 0xFF
        if k == 27:
            break
    else:
        print('error')
        break

cap.release()
ser.close()
cv2.destroyAllWindows()