import requests

response = requests.get("https://api.github.com/repos/elycraft/musicplayer/releases/latest")
print(response.json()["name"])