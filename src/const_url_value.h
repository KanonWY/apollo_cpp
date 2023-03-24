#ifndef __CONST_URL_VALUE_H__
#define __CONST_URL_VALUE_G__

// some url pattern
static const char *NO_BUFFER_URL = "%s/configs/%s/%s/%s"; //config_server_url, appidName
static const char *CONFIGURATIONS_NAME = "configurations";
static const char *NOTIFICATIONS_URL = "%s/notifications/v2"; // config url

// string Name depend on the api standard
static const char *NAMESPACE_STRING_NAME = "namespaceName";
static const char *NOTIFY_STRING_NAME = "notificationId";
static const char *TMP_JSON_KEY_STRING_NAME = "param";
static const char *QUERY_PARAM_NOTIFY_STRING_NAME = "notifications";
static const char *QUERY_PARAM_APPID_STRING_NAME = "appId";
static const char *QUERY_PARAM_CLUSTER_STRING_NAME = "cluster";

#endif