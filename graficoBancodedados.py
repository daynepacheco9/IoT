import datetime
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
    "https": "http://disrct:ets%40bosch207@rb-proxy-ca1.bosch.com:8080"
}

auth = requests.auth.HTTPProxyAuth("disrct","ets@bosch207")

def update_data():
    dados= json.loads(get_request(url, proxy,auth))
    dados_len = len(dados)
    indices = np.array([int(x[-2:]) for x in dados.keys()])

    luminosity = np.full(dados_len, np.nan, dtype=np.float64)
    temp_sensor_00 = np.full(dados_len, np.nan, dtype=np.float64)
    temp_sensor_01 = np.full(dados_len, np.nan, dtype=np.float64)

    for i,j in zip(range(dados_len), indices):
        try: 
            luminosity[i] = dados[f"subsys_{j:02}"]["luminosity"]
            temp_sensor_00[i] = dados[f"subsys_{j:02}"]["temperature"]["temp_sensor_00"]
            temp_sensor_01[i] = dados[f"subsys_{j:02}"]["temperature"]["temp_sensor_01"]    
        except KeyError:
            pass
    
    luminosity_mean = np.mean(luminosity[~np.isnan(luminosity)])
    temp_sensor_00_mean = np.mean(temp_sensor_00[~np.isnan(temp_sensor_00)])
    temp_sensor_01_mean = np.mean(temp_sensor_01[~np.isnan(temp_sensor_01)])
    
    return luminosity_mean, temp_sensor_00_mean , temp_sensor_01_mean
    
  
fig, axs = plt.subplots(3,1, figsize=(16,8),gridspec_kw={"hspace":0.4})
fig.supxlabel("Indices")

ax_luminosity, ax_temp_sensor_00, ax_temp_sensor_01 = axs

ax_luminosity.grid()
ax_luminosity.set_ylabel("luminosity")

ax_temp_sensor_00.grid()
ax_temp_sensor_00.set_ylabel("Temperature sensor 0")  
        
ax_temp_sensor_01.grid()
ax_temp_sensor_01.set_ylabel("Temperature sensor 1")

while True:
    tempo_atual = datetime.datetime.now()
    luminosity_maen,temp_sensor_00_mean, temp_sensor_01_mean = update_data()
    
    ax_luminosity.plot(tempo_atual, luminosity_maen, linestyle='', marker='o', markersize = 5, color='m')    
    ax_temp_sensor_00.plot(tempo_atual, temp_sensor_00_mean, linestyle='', marker='o', markersize = 5, color='m')
    ax_temp_sensor_01.plot(tempo_atual, temp_sensor_01_mean, linestyle='', marker='o', markersize = 5, color='m')
    
    fig.canvas.draw()
    plt.pause(1)

# url_image = dados["url"]

# with open("cachorro.jpg", "wb") as image:
#     req_image = requests.get(url_image, proxies = proxy)
    
#     if req_image.status_code != 200:
#         raise Exception("Erro na requisicao")
    
#     image.write(req_image.content)
 
