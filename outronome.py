import json
import matplotlib.pyplot as plt
import numpy as np
import requests

def get_request(url, proxy):
    req = requests.get(url, proxies = proxy)
    if req.status_code != 200:
        raise Exception("Erro na requisicao")
    return req.content


url = "https://random.dog/woof.json"

proxy = {
    "http": "http://disrct:ets%40bosch207@rb-proxy-ca1.bosch.com:8080",
    "https": "https://disrct:ets%40bosch207@rb-proxy-ca1.bosch.com:8080"
}

#auth = requests.auth.HTTPProxyAuth("disrct","ets@bosch207")



dados= json.loads(get_request(url, proxy))


print(dados)
# url_image = dados["url"]

# with open("cachorro.jpg", "wb") as image:
#     req_image = requests.get(url_image, proxies = proxy)
    
#     if req_image.status_code != 200:
#         raise Exception("Erro na requisicao")
    
#     image.write(req_image.content)

