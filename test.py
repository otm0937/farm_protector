import cv2
from ultralytics import YOLO

url = 'https://www.youtube.com/watch?v=g2sWK3Obmuo&t=41s&ab_channel=MBClife'

model = YOLO('runs/detect/train/weights/best.pt')

result = model.track(source=url, show=True)
