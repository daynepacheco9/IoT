import json
import matplotlib.pyplot as plt
import numpy as np
import requests

def get_request(url, proxy, auth):
    
    
    req = requests.get(url, proxies = proxy, auth = auth)
    if req.status_code != 200:
        raise Exception("Erro na requisicao")
    return req.content


url = "https://iiot-dta-default-rtdb.firebaseio.com/iiot-dta.json"

proxy = {
    "http": "http://disrct:ets%40bosch207@rb-proxy-ca1.bosch.com:8080",
    "https": "https://disrct:ets%40bosch207@rb-proxy-ca1.bosch.com:8080"
}

auth = requests.auth.HTTPProxyAuth("disrct","ets@bosch207")

dados= json.loads(get_request(url, proxy,auth))
dados_len = len(dados)
indices = np.array([int(x[-2:]) for x in dados.keys()])

luminosity = np.full(dados_len, np.nan, dtype=np.float64)
temp_sensor_00 = np.full(dados_len, np.nan, dtype=np.float64)
temp_sensor_01 = np.full(dados_len, np.nan, dtype=np.float64)

for i,j in zip(range(dados_len), indices):
    try: 
        luminosity[i] = dados[f"subsys_{j:02}"]["luminosity"]
        temp_sensor_00 = dados[f"subsys_{j:02}"]["temperature"]["temp_sensor_00"]
        temp_sensor_01 = dados[f"subsys_{j:02}"]["temperature"]["temp_sensor_01"]    
    except KeyError:
      pass
  
fig, axs = plt.subplots(3,1, figsize=(16,8),gridspec_kw={"hspace":0.4})
fig.supxlabel("Indices")

ax_luminosity, ax_temp_sensor_00, ax_temp_sensor_01 = axs

ax_luminosity.grind()
ax_luminosity.set_ylabel("luminosity")

ax_temp_sensor_00.grid()
ax_temp_sensor_00.set_ylabel("Temperature sensor 0")  
        
ax_temp_sensor_01.grid()
ax_temp_sensor_01.set_ylabel("Temperature sensor 1")


ax_luminosity.plot(indices, luminosity, linestyle='', marker='o', markersize = 5, color='m')    
ax_temp_sensor_00.plot(indices, temp_sensor_00, linestyle='', marker='o', markersize = 5, color='m')
ax_temp_sensor_01.plot(indices, temp_sensor_01, linestyle='', marker='o', markersize = 5, color='m')

# url_image = dados["url"]

# with open("cachorro.jpg", "wb") as image:
#     req_image = requests.get(url_image, proxies = proxy)
    
#     if req_image.status_code != 200:
#         raise Exception("Erro na requisicao")
    
#     image.write(req_image.content)
 
