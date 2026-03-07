import json

data = {
   "1": {"lat": 13.650660999999998, "lon": 100.49216416666668, "odom": [-0.05, 0.22]},
   "2": {"lat": 13.650390999999997, "lon": 100.4921555, "odom": [-2.24, 26.48]},
   "4": {"lat": 13.6501445, "lon": 100.49245133333334, "odom": [-37.04, 52.05]},
   "6": {"lat": 13.650231, "lon": 100.49278083333333, "odom": [-70.35, 36.33]},
   "7": {"lat": 13.650536333333331, "lon": 100.49283566666666, "odom": [-77.29, 5.16]}
}

print("Raw GPS Points:")
for k, v in data.items():
    print(f"Vertex {k}: Lat {v['lat']}, Lon {v['lon']} -> Odom: {v['odom']}")

import math

def ll2xy(lat, lon, origin_lat, origin_lon):
    R = 6371000 # Earth radius
    x = R * math.cos(origin_lat * math.pi / 180.0) * (lon - origin_lon) * math.pi / 180.0
    y = R * (lat - origin_lat) * math.pi / 180.0
    return x, y

origin_lat = data["1"]["lat"]
origin_lon = data["1"]["lon"]

print("\nConverted GPS to Local XY (Origin at Vertex 1):")
for k, v in data.items():
    gx, gy = ll2xy(v['lat'], v['lon'], origin_lat, origin_lon)
    print(f"Vertex {k}: GPS_XY({gx:.2f}, {gy:.2f}) <--> ODOM_XY({v['odom'][0]:.2f}, {v['odom'][1]:.2f})")

import numpy as np

def compute_svd(pts_l, pts_g):
    pts_l = np.array(pts_l)
    pts_g = np.array(pts_g)
    
    c_l = np.mean(pts_l, axis=0)
    c_g = np.mean(pts_g, axis=0)
    
    H = np.zeros((2,2))
    for i in range(len(pts_l)):
        H += np.outer(pts_l[i] - c_l, pts_g[i] - c_g)
        
    U, S, Vt = np.linalg.svd(H)
    R = Vt.T @ U.T
    
    if np.linalg.det(R) < 0:
        Vt[1, :] *= -1
        R = Vt.T @ U.T
        
    t = c_g - R @ c_l
    return R, t

pts_odom = [v['odom'] for v in data.values()]
pts_gps_xy = [ll2xy(v['lat'], v['lon'], origin_lat, origin_lon) for v in data.values()]

# We want to transform GPS -> Odom (so Lidar/Odom is Target 'g', GPS is Source 'l')
R, t = compute_svd(pts_gps_xy, pts_odom)

print("\n--- Calculated Matrix (GPS -> ODOM) ---")
print(f"R:\n{R}")
print(f"t:\n{t}")

print("\n--- Verify Alignment ---")
for i, gps in enumerate(pts_gps_xy):
    aligned = R @ np.array(gps) + t
    print(f"GPS {gps} -> Aligned {aligned} | Actual Odom {pts_odom[i]}")
