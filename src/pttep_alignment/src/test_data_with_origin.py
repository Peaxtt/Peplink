import numpy as np
import math

data = {
   "1": {"lat": 13.650660999999998, "lon": 100.49216416666668, "odom": [-0.05, 0.22]},
   "2": {"lat": 13.650390999999997, "lon": 100.4921555, "odom": [-2.24, 26.48]},
   "4": {"lat": 13.6501445, "lon": 100.49245133333334, "odom": [-37.04, 52.05]},
   "6": {"lat": 13.650231, "lon": 100.49278083333333, "odom": [-70.35, 36.33]},
   "7": {"lat": 13.650536333333331, "lon": 100.49283566666666, "odom": [-77.29, 5.16]}
}

def ll2xy(lat, lon, origin_lat, origin_lon):
    R = 6378137.0 # WGS84
    lat_rad = math.radians(lat)
    lon_rad = math.radians(lon)
    origin_lat_rad = math.radians(origin_lat)
    origin_lon_rad = math.radians(origin_lon)
    
    x = R * math.cos(origin_lat_rad) * (lon_rad - origin_lon_rad)
    y = R * (lat_rad - origin_lat_rad)
    return x, y

origin_lat = data["1"]["lat"]
origin_lon = data["1"]["lon"]

pts_odom = [v['odom'] for v in data.values()]
pts_gps_xy = [ll2xy(v['lat'], v['lon'], origin_lat, origin_lon) for v in data.values()]

def compute_weighted_svd(pts_l, pts_g, weights):
    pts_l = np.array(pts_l)
    pts_g = np.array(pts_g)
    
    # Calculate weighted centroids
    sum_w = sum(weights)
    c_l = np.sum([weights[i] * pts_l[i] for i in range(len(pts_l))], axis=0) / sum_w
    c_g = np.sum([weights[i] * pts_g[i] for i in range(len(pts_g))], axis=0) / sum_w
    
    H = np.zeros((2,2))
    for i in range(len(pts_l)):
        H += weights[i] * np.outer(pts_l[i] - c_l, pts_g[i] - c_g)
        
    U, S, Vt = np.linalg.svd(H)
    R = Vt.T @ U.T
    
    if np.linalg.det(R) < 0:
        Vt[1, :] *= -1
        R = Vt.T @ U.T
        
    t = c_g - R @ c_l
    return R, t

weights = [1.0, 1.0, 1.0, 1.0, 1.0] 
R, t = compute_weighted_svd(pts_gps_xy, pts_odom, weights)

print("\n=== FINAL TRANSFORMATION (GPS to ODOM) ===")
print("Translation (t):")
print(f"X: {t[0]:.4f}")
print(f"Y: {t[1]:.4f}")

yaw = math.atan2(R[1,0], R[0,0])
print(f"\nRotation (R):")
print(f"Yaw (rad): {yaw:.4f}")
print(f"Yaw (deg): {math.degrees(yaw):.4f}")
print(f"R Matrix:\n{R}")

print("\n=== COVARIANCE ROTATION TEST ===")
# Fake GPS covariance from point 1 (North, East)
cov_gps = np.array([
    [15.6816, 0.0],
    [0.0, 15.6816]
])
# Rotate it: Cov_new = R * Cov_gps * R^T
cov_aligned = R @ cov_gps @ R.T
print(f"Original GPS Covariance:\n{cov_gps}")
print(f"Rotated Aligned Covariance:\n{cov_aligned}")

