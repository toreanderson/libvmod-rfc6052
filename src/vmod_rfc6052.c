#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#include "vcl.h"
#include "vrt.h"
#include "cache/cache.h"

#include "vcc_if.h"

#include "vsa.h"

int init(const struct vrt_ctx *ctx, struct vmod_priv *priv,
	 enum vcl_event_e e) {
	/* Default to the Well-Known Prefix 64:ff9b::/96 */
	if(!priv->priv)
		vmod_prefix(ctx, priv, "64:ff9b::");
	return 0;
}

/* Set/change the RFC6052 translation prefix used */
VCL_VOID vmod_prefix(VRT_CTX, struct vmod_priv *priv, VCL_STRING prefix) {
	if(!priv->priv) {
		priv->priv = malloc(sizeof(struct in6_addr));
		priv->free = free;
	}

	AN(priv->priv);
	struct in6_addr *ina6 = priv->priv;
	AN(inet_pton(AF_INET6, prefix, ina6));
}

/* Checks if "ip" contains an RFC6052 IPv4-embedded IPv6 address */
VCL_BOOL vmod_is_v4embedded(VRT_CTX, struct vmod_priv *priv, VCL_IP ip) {
	socklen_t sl;
	struct sockaddr_in6 *sin6;
	struct in6_addr *prefix = priv->priv;
	AN(prefix);

	if(!(sin6 = (struct sockaddr_in6 *) VSA_Get_Sockaddr(ip, &sl)))
		return 0;

	if(sin6->sin6_family != AF_INET6)
		return 0;

	if(sin6->sin6_addr.s6_addr32[0] == prefix->s6_addr32[0] &&
	   sin6->sin6_addr.s6_addr32[1] == prefix->s6_addr32[1] &&
	   sin6->sin6_addr.s6_addr32[2] == prefix->s6_addr32[2])
		return 1;
	else
		return 0;
}

/* Extracts an IPv4-embedded address from "ip" and returns it */
VCL_IP vmod_extract(VRT_CTX, struct vmod_priv *priv, VCL_IP ip) {
	socklen_t sl;
	struct sockaddr_in sin;
	struct sockaddr_in6 *sin6;
	void *p;

	if(!vmod_is_v4embedded(ctx, priv, ip))
		return NULL;

	if(!(sin6 = (struct sockaddr_in6 *) VSA_Get_Sockaddr(ip, &sl)))
		return NULL;

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = sin6->sin6_addr.s6_addr32[3];
	sin.sin_port = sin6->sin6_port;

	if(!WS_Reserve(ctx->ws, sizeof(VCL_IP)))
		return NULL;
	p = ctx->ws->f;

	VSA_Build(p, &sin, sizeof(sin));

	WS_Release(ctx->ws, sizeof(VCL_IP));

	return p;
}

/* In-place replacement of "ip" with its IPv4-embedded address */
VCL_VOID vmod_replace(VRT_CTX, struct vmod_priv *priv, VCL_IP ip) {
	socklen_t sl;
	struct sockaddr_in sin;
	struct sockaddr_in6 *sin6;

	if(!vmod_is_v4embedded(ctx, priv, ip))
		return;

	if(!(sin6 = (struct sockaddr_in6 *) VSA_Get_Sockaddr(ip, &sl)))
		return;

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = sin6->sin6_addr.s6_addr32[3];
	sin.sin_port = sin6->sin6_port;

	VSA_Build((void*)ip, &sin, sizeof(sin));
}
