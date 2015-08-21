<?php

/**
 * @file
 * Administration page callbacks for the trade module.
 */


function trade_admin_config_form() {
  $node_type_opts = node_type_get_names();

  $form['trade'] = array(
    '#type' => 'vertical_tabs',
    '#title' => t('Currency'),
    '#default_tab' => 'defaults',
  );

  $form['trade']['currency'] = array(
    '#type' => 'fieldset',
    '#title' => t('Currency'),
  );

  $form['trade']['currency']['trade_currencies'] = array(
    '#type' => 'textarea',
    '#title' => t('Currencies'),
    '#default_value' => variable_get('trade_currencies', _trade_get_currencies('default')),
    '#description' => t('Abbr|suffix|unicode'),
  );

  $form['trade']['currency']['trade_currency'] = array(
    '#type' => 'select',
    '#title' => t('Use currency'),
    '#default_value' => variable_get('trade_currency', 'RUB'),
    '#options' => _trade_get_currencies(),
  );

  $form['trade']['currency']['trade_debug_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Debug mode'),
    '#default_value' => variable_get('trade_debug_mode', FALSE),
  );

//format number settings
  $form['trade']['format_number'] = array(
    '#type' => 'fieldset',
    '#title' => t('Format number'),
  );

  $form['trade']['format_number']['trade_format_number_separator'] = array(
    '#type' => 'select',
    '#title' => t('Separator'),
    '#default_value' => variable_get('trade_format_number_separator', 'space'),
    '#options' => array(
      '_none' => t('Not use'),
      'space' => 'Space: " "',
      'comma' => 'comma: ","',
      'quotes' => 'quotes: " \' "'
    ),
  );

  $form['trade']['product_node_types'] = array(
    '#type' => 'fieldset',
    '#title' => t('Product types'),
  );

  $form['trade']['product_node_types']['trade_product_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Product node types'),
    '#options' => $node_type_opts,
    '#default_value' => variable_get('trade_product_node_types', array('product'))
  );

  // Sales process
  $form['trade']['sales_peocess'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sales process'),
  );

  $form['trade']['sales_peocess']['trade_sales_process_after_add_product_goto_front'] = array(
    '#type' => 'checkbox',
    '#title' => t('After add product to cart, goto front page'),
    '#default_value' => variable_get('trade_sales_process_after_add_product_goto_front', FALSE)
  );

  $form['trade_product_node_types'] = array(
    '#type' => 'vertical_tabs',
    '#default_tab' => 'defaults',
  );
  $trade_product_node_types = array_filter(variable_get('trade_product_node_types', array()));
  // $field_info_fields = field_info_fields();
  foreach ($trade_product_node_types as $trade_product_node_type) {
    $form['trade_product_node_types'][$trade_product_node_type] = array(
      '#type' => 'fieldset',
      '#title' => $node_type_opts[$trade_product_node_type],
    );
    // get field instances
    $field_info_instances = _trade_get_field_instances('node', $trade_product_node_type, array(
      'entityreference',
      'taxonomy_term_reference'
    ));

    if (!empty($field_info_instances)) {
      $form['trade_product_node_types'][$trade_product_node_type]['trade_product_node_types_options_' . $trade_product_node_type] = array(
        '#type' => 'checkboxes',
        '#title' => t('Product trade options field'),
        '#default_value' => variable_get('trade_product_node_types_options_' . $trade_product_node_type, array()),
        '#options' => $field_info_instances,
      );

      //Furniture upholstery fields
      $trade_product_node_types_options = array_filter(variable_get('trade_product_node_types_options_' . $trade_product_node_type, array()));
      if ($trade_product_node_types_options) {

        $upholstery_field_exists = FALSE;
        foreach ($field_info_instances as $field_info_instances_key => $field_info_instances_field) {
          if (strpos($field_info_instances_key, '_upholster')) {
            $upholstery_field_exists = TRUE;
            break;
          }
        }
        if ($upholstery_field_exists) {
          $form['trade_product_node_types'][$trade_product_node_type]['trade_product_node_types_upholstery_options_' . $trade_product_node_type] = array(
            '#type' => 'checkboxes',
            '#title' => t('Product trade upholstery options field'),
            '#default_value' => variable_get('trade_product_node_types_upholstery_options_' . $trade_product_node_type, array()),
            '#options' => array_intersect_key($field_info_instances, $trade_product_node_types_options),
          );

          $form['trade_product_node_types'][$trade_product_node_type]['trade_product_fabricator_field'] = array(
            '#type' => 'select',
            '#title' => t('Product trade fabricator field'),
            '#default_value' => variable_get('trade_product_fabricator_field', 'field_product_fabricator'),
            '#options' => array(t('None')) + array_diff_key($field_info_instances, $trade_product_node_types_options),
          );
        }
        // trade_product_option_color_picker field
        $trade_product_up_opt_fields = array_filter(variable_get('trade_product_node_types_upholstery_options_' . $trade_product_node_type, array()));
        $form['trade_product_node_types'][$trade_product_node_type]['trade_product_option_color_picker_field_' . $trade_product_node_type] = array(
          '#type' => 'select',
          '#title' => t('Product trade color picker field'),
          '#default_value' => variable_get('trade_product_option_color_picker_field_' . $trade_product_node_type, ''),
          '#options' => array(t('None')) + array_diff_key(array_intersect_key($field_info_instances, $trade_product_node_types_options), $trade_product_up_opt_fields),
        );
      }

      //Furniture upholstery collection field
      $trade_product_node_types_upholstery_options = array_filter(variable_get('trade_product_node_types_upholstery_options_' . $trade_product_node_type, array()));
      if (!empty($trade_product_node_types_upholstery_options)) {
        $trade_product_node_types_upholstery_option_field = key($trade_product_node_types_upholstery_options);
        $field_info_field = field_info_field($trade_product_node_types_upholstery_option_field);

        if (!empty($field_info_field['settings']['handler_settings']['target_bundles'])) {
          $material_bundle = key($field_info_field['settings']['handler_settings']['target_bundles']);
          $trade_product_upholstery_field_instances = _trade_get_field_instances('node', $material_bundle, array('taxonomy_term_reference'));
          if ($trade_product_upholstery_field_instances) {
            $form['trade_product_node_types'][$trade_product_node_type]['trade_product_upholstery_collection_field'] = array(
              '#type' => 'select',
              '#title' => t('Product trade upholstery collection field'),
              '#default_value' => variable_get('trade_product_upholstery_collection_field', 'field_material_upholster_collect'),
              '#options' => array(t('None')) + $trade_product_upholstery_field_instances,
            );
          }
        }
      }
    }

  }


  return system_settings_form($form);
}

function trade_admin_config_product_form() {
  global $language;

  $form['vt'] = array(
    '#type' => 'vertical_tabs',
    '#title' => t('Service'),
    '#default_tab' => 'service',
  );

  $form['vt'][TRADE_SERVICE_SHIPPING_TYPE] = array(
    '#type' => 'fieldset',
    '#title' => t('Shipping'),
    '#description' => t('Product default value')
  );
  $form['vt'][TRADE_SERVICE_SHIPPING_TYPE]['fix_amount'][TRADE_SERVICE_SHIPPING_TYPE . '_fix_amount_' . '_amount'] = array(
    '#type' => 'textfield',
    '#title' => t('On city (amount)'),
    '#size' => 12,
    '#maxlength' => 12,
    '#default_value' => variable_get(TRADE_SERVICE_SHIPPING_TYPE . '_fix_amount_' . '_amount', 0),
    '#description' => t('0 - free'),
    '#element_validate' => array('element_validate_number'),
  );
  $form['vt'][TRADE_SERVICE_SHIPPING_TYPE]['fix_amount'][TRADE_SERVICE_SHIPPING_TYPE . '_fix_amount_' . '_update'] = array(
    '#type' => 'checkbox',
    '#title' => t('Update in all products'),
    '#default_value' => FALSE,
  );
  $form['vt'][TRADE_SERVICE_SHIPPING_TYPE]['price_on_distance'][TRADE_SERVICE_SHIPPING_TYPE . '_price_on_distance_' . '_amount'] = array(
    '#type' => 'textfield',
    '#title' => t('From city (price 1 km)'),
    '#size' => 12,
    '#maxlength' => 12,
    '#default_value' => variable_get(TRADE_SERVICE_SHIPPING_TYPE . '_price_on_distance_' . '_amount', 0),
    '#description' => t('0 - free'),
    '#element_validate' => array('element_validate_number'),
  );
  $form['vt'][TRADE_SERVICE_SHIPPING_TYPE]['price_on_distance'][TRADE_SERVICE_SHIPPING_TYPE . '_price_on_distance_' . '_update'] = array(
    '#type' => 'checkbox',
    '#title' => t('Update in all products'),
    '#default_value' => FALSE,
  );

  //custom shipping
  if (db_table_exists('trade_shipping')) {
    $trade_shippings = trade_shipping_load_multiply();
    foreach ($trade_shippings as $shipping_key => $trade_shipping) {
      $trade_shipping_key = TRADE_SERVICE_SHIPPING_TYPE . '_custom_' . $trade_shipping->sid;
      $form['vt'][TRADE_SERVICE_SHIPPING_TYPE][$trade_shipping_key] = array(
        '#type' => 'fieldset',
        '#title' => $trade_shipping->title,
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      $form['vt'][TRADE_SERVICE_SHIPPING_TYPE][$trade_shipping_key][TRADE_SERVICE_SHIPPING_TYPE . "_custom_{$trade_shipping->sid}_title"] = array(
        '#type' => 'textfield',
        '#title' => t('Title'),
        '#default_value' => $trade_shipping->title,
      );
      $form['vt'][TRADE_SERVICE_SHIPPING_TYPE][$trade_shipping_key][TRADE_SERVICE_SHIPPING_TYPE . "_custom_{$trade_shipping->sid}_amount"] = array(
        '#type' => 'textfield',
        '#title' => t('Amount'),
        '#default_value' => $trade_shipping->amount,
        '#required' => TRUE,
        '#element_validate' => array('element_validate_number')
      );
      $form['vt'][TRADE_SERVICE_SHIPPING_TYPE][$trade_shipping_key][TRADE_SERVICE_SHIPPING_TYPE . "_custom_{$trade_shipping->sid}_weight"] = array(
        '#type' => 'textfield',
        '#title' => t('Weight'),
        '#default_value' => $trade_shipping->weight,
        '#required' => TRUE,
        '#element_validate' => array('element_validate_number')
      );
      $form['vt'][TRADE_SERVICE_SHIPPING_TYPE][$trade_shipping_key][TRADE_SERVICE_SHIPPING_TYPE . "_custom_{$trade_shipping->sid}_delete"] = array(
        '#type' => 'checkbox',
        '#title' => t('Delete'),
        '#default_value' => FALSE,
      );
    }

    $form['vt'][TRADE_SERVICE_SHIPPING_TYPE][TRADE_SERVICE_SHIPPING_TYPE . '_custom_add_shipping'] = array(
      '#type' => 'fieldset',
      '#title' => t('Add shipping'),
    );

    $form['vt'][TRADE_SERVICE_SHIPPING_TYPE][TRADE_SERVICE_SHIPPING_TYPE . '_custom_add_shipping'][TRADE_SERVICE_SHIPPING_TYPE . '_custom_add_shipping_title'] = array(
      '#type' => 'textfield',
      '#title' => t('Title'),
    );

    $form['vt'][TRADE_SERVICE_SHIPPING_TYPE][TRADE_SERVICE_SHIPPING_TYPE . '_custom_add_shipping'][TRADE_SERVICE_SHIPPING_TYPE . '_custom_add_shipping_amount'] = array(
      '#type' => 'textfield',
      '#title' => t('Amount'),
      '#element_validate' => array('element_validate_number')
    );
    $form['vt'][TRADE_SERVICE_SHIPPING_TYPE][TRADE_SERVICE_SHIPPING_TYPE . '_custom_add_shipping'][TRADE_SERVICE_SHIPPING_TYPE . '_custom_add_shipping_weight'] = array(
      '#type' => 'textfield',
      '#title' => t('Weight'),
      '#element_validate' => array('element_validate_number')
    );
  }

  $form['vt'][TRADE_SERVICE_LIFTING_TYPE] = array(
    '#type' => 'fieldset',
    '#title' => t('Lifting'),
  );

  $form['vt'][TRADE_SERVICE_LIFTING_TYPE]['lift'][TRADE_SERVICE_LIFTING_TYPE . '_lift'] = array(
    '#type' => 'select',
    '#title' => t('Lift'),
    '#options' => array(
      '_none' => t('Without lifting'),
      'free' => t('Free'),
      'amount' => t('Fixed amount'),
      'percent_on_amount' => t('3%, not less 200 rub.'),
    ),
    '#default_value' => variable_get(TRADE_SERVICE_LIFTING_TYPE . '_lift', 'free'),
  );

  $form['vt'][TRADE_SERVICE_LIFTING_TYPE]['lift'][TRADE_SERVICE_LIFTING_TYPE . '_lift_' . '_amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Fixed amount'),
    '#size' => 12,
    '#maxlength' => 12,
    '#default_value' => variable_get(TRADE_SERVICE_LIFTING_TYPE . '_lift_' . '_amount', 0),
    '#element_validate' => array('element_validate_number'),
  );

  $form['vt'][TRADE_SERVICE_LIFTING_TYPE]['lift'][TRADE_SERVICE_LIFTING_TYPE . '_steps'] = array(
    '#type' => 'select',
    '#title' => t('On stairs'),
    '#options' => array(
      '_none' => t('Without lifting'),
      'free' => t('Free'),
      'price' => t('Fix price on floor'),
      'percent_on_amount' => t('3%, not less 250 rub. on floor'),
    ),
    '#default_value' => variable_get(TRADE_SERVICE_LIFTING_TYPE . '_steps', 'percent_on_amount'),
  );

  $form['vt'][TRADE_SERVICE_LIFTING_TYPE]['lift'][TRADE_SERVICE_LIFTING_TYPE . '_steps_' . '_price'] = array(
    '#type' => 'textfield',
    '#title' => t('Fix price on floor'),
    '#size' => 12,
    '#maxlength' => 12,
    '#default_value' => variable_get(TRADE_SERVICE_LIFTING_TYPE . '_steps_' . '_price', 0),
    '#element_validate' => array('element_validate_number'),
  );

  // assembly
  $form['vt'][TRADE_SERVICE_ASSEMBLY_TYPE] = array(
    '#type' => 'fieldset',
    '#title' => t('Assembly'),
  );

  $form['vt'][TRADE_SERVICE_ASSEMBLY_TYPE][TRADE_SERVICE_ASSEMBLY_TYPE] = array(
    '#type' => 'select',
    '#title' => t('Assembly type'),
    '#options' => array(
      '_none' => t('With out assembly'),
      'amount' => t('Amount'),
      'percent_on_amount_15' => t('15%, not less 500 rub.'),
      'percent_on_amount_10' => t('10%, not less 700 rub.'),
    ),
    '#default_value' => variable_get(TRADE_SERVICE_ASSEMBLY_TYPE, '_none'),
  );
  $form['vt'][TRADE_SERVICE_ASSEMBLY_TYPE][TRADE_SERVICE_ASSEMBLY_TYPE . '_assembly_amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Amount'),
    '#size' => 12,
    '#maxlength' => 12,
    '#default_value' => variable_get(TRADE_SERVICE_ASSEMBLY_TYPE . '_assembly_amount', 0),
    '#element_validate' => array('element_validate_number'),
  );
  $form['#submit'][] = 'trade_admin_config_product_form_submit';
  return system_settings_form($form);
}

function trade_admin_config_product_form_submit($form, &$form_state) {

  //update fix_amount in all products
  if (!empty($form_state['values'][TRADE_SERVICE_SHIPPING_TYPE . '_fix_amount_' . '_update'])) {
    $amount = $form_state['values'][TRADE_SERVICE_SHIPPING_TYPE . '_fix_amount_' . '_amount'];
    foreach (db_select('trade_service', 'tse')->fields('tse')->execute()->fetchAll() as $service) {
      $data = unserialize($service->data);
      $data['trade_shipping']['fix_amount']['amount'] = $amount;
      $service->data = serialize($data);
      db_update('trade_service')->fields((array) $service)->condition('nid', $service->nid)->execute();
    }
  }

  //update price_on_distance in all products
  if (!empty($form_state['values'][TRADE_SERVICE_SHIPPING_TYPE . '_price_on_distance_' . '_update'])) {
    $amount = $form_state['values'][TRADE_SERVICE_SHIPPING_TYPE . '_price_on_distance_' . '_amount'];
    foreach (db_select('trade_service', 'tse')->fields('tse')->execute()->fetchAll() as $service) {
      $data = unserialize($service->data);
      $data['trade_shipping']['price_on_distance']['price'] = $amount;
      $service->data = serialize($data);
      db_update('trade_service')->fields((array) $service)->condition('nid', $service->nid)->execute();
    }
  }

  // custom shipping
  if (db_table_exists('trade_shipping')) {
    foreach ($form_state['values'] as $val_key => $val) {
      if (strpos($val_key, TRADE_SERVICE_SHIPPING_TYPE . '_custom_') === 0) {
        $search = TRADE_SERVICE_SHIPPING_TYPE . '_custom_';
        $shipping = str_replace($search, '', $val_key);
        $shipping_ex = explode('_', $shipping);
        if (count($shipping_ex) == 2 && is_numeric($shipping_ex[0])) {
          $sid = $shipping_ex[0];
          $field_key = $shipping_ex[1];
          $fields = array(
            $field_key => trim(filter_xss($val, array())),
          );
          if ($field_key == 'delete') {
            if (!empty($val)) {
              db_delete('trade_shipping')->condition('sid', $sid)->execute();

              //delete custom shipping from product services
              foreach (db_select('trade_service', 'tse')->fields('tse')->execute()->fetchAll() as $service) {
                $data = unserialize($service->data);
                unset($data['trade_shipping']['custom_' . $sid]);
                $service->data = serialize($data);
                db_update('trade_service')->fields((array) $service)->condition('nid', $service->nid)->execute();
              }
            }
          }
          else {
            db_update('trade_shipping')->fields($fields)->condition('sid', $sid)->execute();
          }
        }
      }
    }

    if (!empty($form_state['values']['trade_shipping_custom_add_shipping_title'])) {
      $record = array(
        'title' => trim(filter_xss($form_state['values']['trade_shipping_custom_add_shipping_title'], array())),
      );
      if (!empty($form_state['values']['trade_shipping_custom_add_shipping_amount'])) {
        $record['amount'] = $form_state['values']['trade_shipping_custom_add_shipping_amount'];
      }
      if (!empty($form_state['values']['trade_shipping_custom_add_shipping_amount'])) {
        $record['weight'] = $form_state['values']['trade_shipping_custom_add_shipping_weight'];
      }
      drupal_write_record('trade_shipping', $record);

      // add in products services
      foreach (db_select('trade_service', 'tse')->fields('tse')->execute()->fetchAll() as $service) {
        $data = unserialize($service->data);
        $data['trade_shipping']['custom_' . $record['sid']]['amount'] = $record['amount'];
        $service->data = serialize($data);
        db_update('trade_service')->fields((array) $service)->condition('nid', $service->nid)->execute();
      }
    }
  }

}

function trade_admin_config_cache_form() {
  global $language;

  $form['trade_product_cache_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Product teaser trade cache enable'),
    '#default_value' => variable_get('trade_product_cache_enable', TRUE),
  );

  $field_material_upholster_collect_name = variable_get('trade_product_upholstery_collection_field', 'field_material_upholster_collect');
  $field_material_upholster_collect_info = field_info_field($field_material_upholster_collect_name);

  if (!empty($field_material_upholster_collect_name) && !empty($field_material_upholster_collect_info)) {
    $form['trade_product_upholstery_cache_enable'] = array(
      '#type' => 'checkbox',
      '#title' => t('Product trade upholstery picker cache enable'),
      '#default_value' => variable_get('trade_product_upholstery_cache_enable', FALSE),
    );
    $form['trade_product_upholstery_ajax_callback_use'] = array(
      '#type' => 'checkbox',
      '#title' => t('Product trade upholstery picker full ajax callback use'),
      '#default_value' => variable_get('trade_product_upholstery_ajax_callback_use', FALSE),
    );
  }
  $cache_count = db_query('SELECT COUNT(*) FROM {trade_pcache}')->fetchField();

  // total product count
  $count = _trade_product_get_count();
  $form['count'] = array(
    '#type' => 'item',
    '#markup' => t('Total product count - @count', array('@count' => $count)),
  );

  $form['clear_cache'] = array(
    '#type' => 'submit',
    '#value' => t('Re-create cache') . " ({$cache_count})",
    '#submit' => array('trade_admin_config_cache_form_submit')
  );

  return system_settings_form($form);
}

function trade_admin_config_cache_form_submit($form, &$form_state) {

  // Reset counter for debug information.
  $_SESSION['http_request_count'] = 0;
  $nids = _trade_product_get_nids();
  $batch = _trade_batch_pcache_batch($nids);
  batch_set($batch);
  drupal_set_message('Product cache cleared');
}