from datetime import datetime as dt, timezone
import sys
import torch
import shutil
import os
from services.firestore_docker import Connection_Docker
# Loading in yolov5s - you can switch to larger models such as yolov5m or yolov5l, or smaller such as yolov5n

WEIGHT_PATH = "/app/yolov5s.pt"
RESULT_PATH = "/app/results"

class Main():

    def __init__(self, file_path):

        self.img_path = file_path
        self.file_name = os.path.basename(self.img_path)
        self.result_dir_blob = "results/"+self.file_name
        self.result_dir_docker = RESULT_PATH+"/"+self.file_name
        print(f"Blob directory for upload image {self.result_dir_blob} ")
        print(f"docker directory for result image {self.result_dir_docker} ")

        model = torch.hub.load('ultralytics/yolov5', 'custom', WEIGHT_PATH)
        model.classes = [0]
        # img = 'data/images/image.jpg'  # or file, Path, PIL, OpenCV, numpy, list
        results = model(self.img_path)

        results.save(save_dir='results')

        df = results.pandas().xyxy[0]  # create dataframe from image result
        print(df)
        # filtered_df = df[df['class'] == 0] # Only take the humans
        # print(filtered_df)

        count_of_people = df.shape[0]
        timestamp = dt.now().strftime('%c')

        print("There is " + str(count_of_people) + " people(s)" + " at " + timestamp)

        persons_sum_dict = {'sum': count_of_people, 'timeAdded': dt.now(timezone.utc)} # Take UTC time because firestore smart enough to convert UTC time.

        Connection_Docker.initialize_sdk(self)
        Connection_Docker.post_doc(self, persons_sum_dict, "persons-sum")

        Connection_Docker.upload_image(self, self.result_dir_docker, self.result_dir_blob)

        # Check if the folder exists
        if os.path.exists(RESULT_PATH):
            # Use shutil.rmtree() to delete the folder and its contents
            shutil.rmtree(RESULT_PATH)
            print(f"Folder '{RESULT_PATH}' and its contents have been deleted.")
        else:
            print(f"The folder '{RESULT_PATH}' does not exist.")
    
if __name__ == "__main__":
    if len(sys.argv) > 1:
        image_path = sys.argv[1]
        main = Main(image_path)
    else:
        print("No image path provided.")