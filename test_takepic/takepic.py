import serial
import struct

# Open the serial connection
ser = serial.Serial('COM9', 115200)  # Replace 'COM3' with your port

while True:
    # Read the length of the image data
    data = ser.read(4)
    img_len = struct.unpack('I', data)[0]
    
    # Read the image data
    img_data = ser.read(img_len)
    
    # Save the image
    with open("captured_image.jpg", "wb") as f:
        f.write(img_data)
    
    print("Image saved as captured_image.jpg")
