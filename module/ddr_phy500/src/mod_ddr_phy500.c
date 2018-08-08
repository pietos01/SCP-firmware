/*
 * Arm SCP/MCP Software
 * Copyright (c) 2015-2018, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Description:
 *     DDR-PHY500 driver
 */

#include <fwk_assert.h>
#include <fwk_errno.h>
#include <fwk_id.h>
#include <fwk_mm.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>
#include <mod_ddr_phy500.h>
#include <mod_dmc500.h>

static struct mod_log_api *log_api;

/*
 * Functions fulfilling this module's interface
 */
static int ddr_phy500_config(fwk_id_t element_id)
{
    int status;
    struct mod_ddr_phy500_reg *ddr;
    const struct mod_ddr_phy500_module_config *module_config;
    const struct mod_ddr_phy500_element_config *element_config;

    status = fwk_module_check_call(element_id);
    if (status != FWK_SUCCESS)
        return status;

    module_config = fwk_module_get_data(fwk_module_id_ddr_phy500);

    element_config = fwk_module_get_data(element_id);

    ddr = (struct mod_ddr_phy500_reg *)element_config->ddr;

    status = log_api->log(MOD_LOG_GROUP_INFO,
        "[DDR] Initializing PHY at 0x%x\n", (uintptr_t) ddr);
    if (status != FWK_SUCCESS)
        return status;

    ddr->T_CTRL_DELAY   = module_config->ddr_reg_val->T_CTRL_DELAY;
    ddr->READ_DELAY     = module_config->ddr_reg_val->READ_DELAY;
    ddr->T_CTRL_UPD_MIN = module_config->ddr_reg_val->T_CTRL_UPD_MIN;
    ddr->DELAY_SEL      = module_config->ddr_reg_val->DELAY_SEL;

    ddr->CAPTURE_MASK  = module_config->ddr_reg_val->CAPTURE_MASK;
    ddr->T_RDDATA_EN   = module_config->ddr_reg_val->T_RDDATA_EN;
    ddr->T_RDLAT       = module_config->ddr_reg_val->T_RDLAT;
    ddr->T_WRLAT       = module_config->ddr_reg_val->T_WRLAT;
    ddr->DFI_WR_PREMBL = module_config->ddr_reg_val->DFI_WR_PREMBL;

    ddr->LP_ACK        = module_config->ddr_reg_val->LP_ACK;

    return FWK_SUCCESS;
}

static struct mod_dmc500_ddr_phy_api ddr_phy500_api = {
    .configure = ddr_phy500_config,
};

/*
 * Functions fulfilling the framework's module interface
 */

static int ddr_phy500_init(fwk_id_t module_id, unsigned int element_count,
                           const void *config)
{
    assert(config != NULL);

    return FWK_SUCCESS;
}

static int ddr_phy500_element_init(fwk_id_t element_id, unsigned int unused,
                                   const void *data)
{
    assert(data != NULL);

    return FWK_SUCCESS;
}

static int ddr_phy500_bind(fwk_id_t id, unsigned int round)
{
    int status;

    /* Skip the second round (rounds are zero-indexed) */
    if (round == 1)
        return FWK_SUCCESS;

    /* Nothing to be done for element-level binding */
    if (fwk_module_is_valid_element_id(id))
        return FWK_SUCCESS;

    /* Bind to the log module and get a pointer to its API */
    status = fwk_module_bind(FWK_ID_MODULE(FWK_MODULE_IDX_LOG), MOD_LOG_API_ID,
        &log_api);
    if (status != FWK_SUCCESS)
        return FWK_E_HANDLER;

    return FWK_SUCCESS;
}

static int ddr_phy500_process_bind_request(fwk_id_t requester_id, fwk_id_t id,
    fwk_id_t api_type, const void **api)
{
    /* Binding to elements is not permitted. */
    if (fwk_module_is_valid_element_id(id))
        return FWK_E_ACCESS;

    *((const void**)api) = &ddr_phy500_api;

    return FWK_SUCCESS;
}

const struct fwk_module module_ddr_phy500 = {
    .name = "DDR-PHY500",
    .type = FWK_MODULE_TYPE_DRIVER,
    .init = ddr_phy500_init,
    .element_init = ddr_phy500_element_init,
    .bind = ddr_phy500_bind,
    .process_bind_request = ddr_phy500_process_bind_request,
    .api_count = 1,
};