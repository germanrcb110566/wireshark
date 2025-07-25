/* packet-ocsp.c
 * Routines for Online Certificate Status Protocol (RFC2560) packet dissection
 *  Ronnie Sahlberg 2004
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "config.h"

#include <epan/packet.h>
#include <wsutil/array.h>

#include <asn1.h>

#include "packet-ber.h"
#include "packet-ocsp.h"
#include "packet-x509af.h"
#include "packet-x509ce.h"
#include "packet-pkix1implicit.h"
#include "packet-pkix1explicit.h"

#define PNAME  "Online Certificate Status Protocol"
#define PSNAME "OCSP"
#define PFNAME "ocsp"

void proto_register_ocsp(void);
void proto_reg_handoff_ocsp(void);

static dissector_handle_t ocsp_request_handle;
static dissector_handle_t ocsp_response_handle;

/* Initialize the protocol and registered fields */
int proto_ocsp;
static int hf_ocsp_responseType_id;
#include "packet-ocsp-hf.c"

/* Initialize the subtree pointers */
static int ett_ocsp;
#include "packet-ocsp-ett.c"

#include "packet-ocsp-fn.c"


static int
dissect_ocsp_request(tvbuff_t *tvb, packet_info *pinfo, proto_tree *parent_tree, void *data _U_)
{
	proto_item *item=NULL;
	proto_tree *tree=NULL;
	asn1_ctx_t asn1_ctx;
	asn1_ctx_init(&asn1_ctx, ASN1_ENC_BER, true, pinfo);

	col_set_str(pinfo->cinfo, COL_PROTOCOL, "OCSP");

	col_set_str(pinfo->cinfo, COL_INFO, "Request");


	if(parent_tree){
		item=proto_tree_add_item(parent_tree, proto_ocsp, tvb, 0, -1, ENC_NA);
		tree = proto_item_add_subtree(item, ett_ocsp);
	}

	return dissect_ocsp_OCSPRequest(false, tvb, 0, &asn1_ctx, tree, -1);
}


static int
dissect_ocsp_response(tvbuff_t *tvb, packet_info *pinfo, proto_tree *parent_tree, void *data _U_)
{
	proto_item *item=NULL;
	proto_tree *tree=NULL;
	asn1_ctx_t asn1_ctx;
	asn1_ctx_init(&asn1_ctx, ASN1_ENC_BER, true, pinfo);

	col_set_str(pinfo->cinfo, COL_PROTOCOL, "OCSP");

	col_set_str(pinfo->cinfo, COL_INFO, "Response");


	if(parent_tree){
		item=proto_tree_add_item(parent_tree, proto_ocsp, tvb, 0, -1, ENC_NA);
		tree = proto_item_add_subtree(item, ett_ocsp);
	}

	return dissect_ocsp_OCSPResponse(false, tvb, 0, &asn1_ctx, tree, -1);
}

/*--- proto_register_ocsp ----------------------------------------------*/
void proto_register_ocsp(void) {

  /* List of fields */
  static hf_register_info hf[] = {
    { &hf_ocsp_responseType_id,
      { "ResponseType Id", "ocsp.responseType.id",
        FT_STRING, BASE_NONE, NULL, 0,
        NULL, HFILL }},
#include "packet-ocsp-hfarr.c"
  };

  /* List of subtrees */
  static int *ett[] = {
    &ett_ocsp,
#include "packet-ocsp-ettarr.c"
  };

  /* Register protocol */
  proto_ocsp = proto_register_protocol(PNAME, PSNAME, PFNAME);

  /* Register fields and subtrees */
  proto_register_field_array(proto_ocsp, hf, array_length(hf));
  proto_register_subtree_array(ett, array_length(ett));

  /* Register dissectors */
  ocsp_request_handle = register_dissector_with_description(PFNAME "_req", PSNAME " Request", dissect_ocsp_request, proto_ocsp);
  ocsp_response_handle = register_dissector_with_description(PFNAME "_res", PSNAME " Response", dissect_ocsp_response, proto_ocsp);
}

/*--- proto_reg_handoff_ocsp -------------------------------------------*/
void proto_reg_handoff_ocsp(void) {
	dissector_add_string("media_type", "application/ocsp-request", ocsp_request_handle);
	dissector_add_string("media_type", "application/ocsp-response", ocsp_response_handle);

#include "packet-ocsp-dis-tab.c"
}

