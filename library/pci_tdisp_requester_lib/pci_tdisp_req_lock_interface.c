/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "hal/base.h"
#include "hal/library/memlib.h"
#include "library/spdm_requester_lib.h"
#include "library/spdm_transport_pcidoe_lib.h"
#include "library/pci_tdisp_requester_lib.h"

/**
 * Send and receive a TDISP message
 *
 * @param  spdm_context                 A pointer to the SPDM context.
 * @param  session_id                   Indicates if it is a secured message protected via SPDM session.
 *                                     If session_id is NULL, it is a normal message.
 *                                     If session_id is NOT NULL, it is a secured message.
 *
 * @retval LIBSPDM_STATUS_SUCCESS               The TDISP request is sent and response is received.
 * @return ERROR                        The TDISP response is not received correctly.
 **/
libspdm_return_t pci_tdisp_lock_interface(const void *pci_doe_context,
                                          void *spdm_context, const uint32_t *session_id,
                                          const pci_tdisp_interface_id_t *interface_id,
                                          const pci_tdisp_lock_interface_param_t *lock_interface_param,
                                          uint8_t *start_interface_nonce)
{
    libspdm_return_t status;
    pci_tdisp_lock_interface_request_t request;
    size_t request_size;
    pci_tdisp_lock_interface_response_t response;
    size_t response_size;

    libspdm_zero_mem (&request, sizeof(request));
    request.header.version = PCI_TDISP_MESSAGE_VERSION_10;
    request.header.message_type = PCI_TDISP_LOCK_INTERFACE_REQ;
    request.header.interface_id.function_id = interface_id->function_id;

    request_size = sizeof(request);
    response_size = sizeof(response);
    status = pci_tdisp_send_receive_data(spdm_context, session_id,
                                         &request, request_size,
                                         &response, &response_size);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        return status;
    }

    if (response_size != sizeof(pci_tdisp_lock_interface_response_t)) {
        return LIBSPDM_STATUS_INVALID_MSG_SIZE;
    }
    if (response.header.version != request.header.version) {
        return LIBSPDM_STATUS_INVALID_MSG_FIELD;
    }
    if (response.header.message_type != PCI_TDISP_LOCK_INTERFACE_RSP) {
        return LIBSPDM_STATUS_INVALID_MSG_FIELD;
    }
    if (response.header.interface_id.function_id != request.header.interface_id.function_id) {
        return LIBSPDM_STATUS_INVALID_MSG_FIELD;
    }

    libspdm_copy_mem (start_interface_nonce, PCI_TDISP_START_INTERFACE_NONCE_SIZE,
                      response.start_interface_nonce, sizeof(response.start_interface_nonce));

    return LIBSPDM_STATUS_SUCCESS;
}
