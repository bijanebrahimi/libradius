/*
 * example RADIUS client
 *
 * send a message to a local RADIUS accounting server
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <radlib.h>
#include <sys/socket.h>
#include <sys/select.h>

#define RAD_SRV_SOCKET	1813	/* accounting server (auth uses 1812) */
#define RAD_SRV_SECRET	"1234"
#define RAD_VENDOR_ID	123456

static int
do_vendor_specific(const char *data)
{
	char *cp = data;
	uint32_t vendor;
	size_t size;

	int type = rad_get_vendor_attr(&vendor, (void *)&data, &size);
	fprintf(stderr, "VSA: %d %u %d %s\n", vendor, size, type, data);

	return 0;
}

static int
do_accept(struct rad_handle *radh)
{
	int attrtype;
	const void *attrval;
	size_t attrlen;
	char *s, *data;

	while ((attrtype = rad_get_attr(radh, &attrval, &attrlen)) > 0) {
		switch (attrtype) {
		case RAD_USER_NAME:
			if ((s = rad_cvt_string(attrval, attrlen)) == NULL)
				goto enomem;
			fprintf(stderr, "RAD_USER_NAME: %s\n", s);
			free(s);
			break;
		case RAD_REPLY_MESSAGE:
			if ((s = rad_cvt_string(attrval, attrlen)) == NULL)
				goto enomem;
			fprintf(stderr, "RAD_USER_NAME: %s\n", s);
			free(s);
			break;
		case RAD_VENDOR_SPECIFIC:
			if ((s = rad_cvt_string(attrval, attrlen)) == NULL)
				goto enomem;
			do_vendor_specific(s);
			free(s);
			break;
		default:
			fprintf(stderr, "ignoring RADIUS attribute %d",
			    __func__, attrtype);
		}
	}
	if (attrtype == -1) {
		fprintf(stderr, "rad_get_attr: %s", rad_strerror(radh));
		return (-1);
	}
	return (0);
enomem:
	fprintf(stderr, "%s(): out of memory", __func__);
	return (-1);
}


int
main(int argc, char **argv)
{
	const char *conf_file = "radius.conf";
	struct rad_handle *radh = NULL;
	const char *user, *pass;
	int rc = 0;

	if (argc != 3) {
		fprintf(stderr, "invalid arguments\n");
		return (-1);
	} else {
		user = argv[1];
		pass = argv[2];
	}

	if (!(radh = rad_auth_open())) {
		fprintf(stderr, "failed to create radius lib handle\n");
		return -1;	
	}

	if (rad_config(radh, conf_file) == -1) {
		fprintf(stderr, "rad_config: %s", rad_strerror(radh));
		rad_close(radh);
		return -1;
	}

	if (rad_create_request(radh, RAD_ACCESS_REQUEST) == -1) {
		fprintf(stderr, "rad_create_request: %s", rad_strerror(radh));
		return (-1);
	}

	if (rad_put_string(radh, RAD_USER_NAME, user) == -1) {
		fprintf(stderr, "rad_put_string: %s", rad_strerror(radh));
		return (-1);
	}

	if (rad_put_string(radh, RAD_USER_PASSWORD, pass) == -1) {
		fprintf(stderr, "rad_put_string: %s", rad_strerror(radh));
		return (-1);
	}

	if (rad_put_int(radh, RAD_SERVICE_TYPE, RAD_AUTHENTICATE_ONLY) == -1) {
		fprintf(stderr, "rad_put_int: %s", rad_strerror(radh));
		return (-1);
	}

	for(;;) {
		rc = rad_send_request(radh);
		switch (rc) {
		case RAD_ACCESS_ACCEPT:
			rc = do_accept(radh);
			rad_close(radh);
			if (rc == -1)
				exit(1);
			exit(0);
			break;
		case RAD_ACCESS_REQUEST:
		case RAD_ACCESS_REJECT:
		case RAD_ACCOUNTING_REQUEST:
		case RAD_ACCOUNTING_RESPONSE:
		case RAD_ACCESS_CHALLENGE:
		case RAD_DISCONNECT_REQUEST:
		case RAD_DISCONNECT_ACK:
		case RAD_DISCONNECT_NAK:
		case RAD_COA_REQUEST:
		case RAD_COA_ACK:
		case RAD_COA_NAK:
			fprintf(stderr, "rad_send_request: %d", rc);
			exit(1);
		}
	}

}
