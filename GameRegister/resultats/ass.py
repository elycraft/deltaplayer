import json
import os

def get_json_files(folder_path):
    json_files = []
    for root, dirs, files in os.walk(folder_path):
        for file in files:
            if file.endswith('.json'):
                json_files.append(os.path.join(root, file))
    return json_files

def load(ff):
    with open(ff,"r") as file:
        h = file.read()
        a = json.loads(h)
    return a

b= []
for x in get_json_files("."):
    b = b + load(x)

print(b, len(b))
with open("final.json","w") as file:
        file.write(json.dumps(b))