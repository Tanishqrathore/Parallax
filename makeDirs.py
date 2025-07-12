import os
import string
import random

def makeFiles(path):
    os.makedirs(path, exist_ok=True)
    os.chdir(path)
    for i in range(1000):
        filename = f"test{i+1}.txt"
        with open(filename, "w") as file:
            random_string = ''.join(random.choices(string.ascii_uppercase + string.digits, k=1000))
            file.write(random_string)

    # ✅ Generate 8 poison pill files (contain only "*")
    for i in range(8):
        poison_filename = f"poison{i+1}.txt"
        with open(poison_filename, "w") as file:
            file.write("*")
        

if __name__ == "__main__":
    makeFiles("test")