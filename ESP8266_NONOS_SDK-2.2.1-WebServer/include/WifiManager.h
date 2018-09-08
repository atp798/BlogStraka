#ifndef __WIFI_MANAGER_H__
#define __WIFI_MANAGER_H__


void WifiInitConfig(void);
bool WifiStationConfig(struct station_config *config);
bool WifiSoftAPConfig(struct softap_config *config );

#endif
