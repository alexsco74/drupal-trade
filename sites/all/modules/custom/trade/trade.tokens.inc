<?php

/**
 * @file
 * Builds placeholder replacement tokens for trade-order-related data.
 */


/**
 * Implements hook_token_info().
 */
function trade_token_info() {
  $type = array(
    'name' => t('Trade orders'),
    'description' => t('Tokens related to individual content items, or "orders".'),
    'needs-data' => 'order',
  );

  // Core tokens for order.
  $order['id'] = array(
    'name' => t("Order ID"),
    'description' => t('The unique ID of the order.'),
  );
  $order['name'] = array(
    'name' => t("Name"),
    'description' => t("Client name from order."),
  );
  $order['phone'] = array(
    'name' => t("Phone"),
    'description' => t("Client phone from order"),
  );
  $order['mail'] = array(
    'name' => t("Mail"),
    'description' => t("Client mail from order"),
  );
  $order['address'] = array(
    'name' => t("Address"),
    'description' => t("The client address from order."),
  );
  $order['comment'] = array(
    'name' => t("Comment"),
    'description' => t("The client comment from order."),
  );
  $order['url'] = array(
    'name' => t("URL"),
    'description' => t("The URL of the order."),
  );

  $order['created'] = array(
    'name' => t("Date created"),
    'description' => t("The date the order was posted (ISO)."),
    'type' => 'date',
  );
  $order['changed'] = array(
    'name' => t("Date changed"),
    'description' => t("The date the order was most recently updated (ISO)."),
    'type' => 'date',
  );

  return array(
    'types' => array('order' => $type),
    'tokens' => array('order' => $order),
  );
}

/**
 * Implements hook_tokens().
 */
function trade_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $url_options = array('absolute' => TRUE);
  if (isset($options['language'])) {
    $url_options['language'] = $options['language'];
    $language_code = $options['language']->language;
  }
  else {
    $language_code = NULL;
  }
  $sanitize = !empty($options['sanitize']);

  $replacements = array();

  if ($type == 'order' && !empty($data['order'])) {
    $order = $data['order'];

    foreach ($tokens as $name => $original) {
      switch ($name) {
        // Simple key values on the order.
        case 'id':
          $replacements[$original] = $order->order_id;
          break;

        case 'name':
          $replacements[$original] = $order->name;
          break;

        case 'phone':
          $replacements[$original] = $order->phone;
          break;

        case 'mail':
          $replacements[$original] = $order->mail;
          break;

        case 'address':
          $replacements[$original] = $order->address;
          break;

        case 'comment':
          $replacements[$original] = $order->comment;
          break;

        case 'url':
          $replacements[$original] = url('trade/order/' . $order->order_id, $url_options);
          break;

        case 'created':
          $time = strtotime($order->created);
          $replacements[$original] = format_date($time, 'medium', '', NULL, $language_code);
          break;

        case 'changed':
          $time = strtotime($order->changed);
          $replacements[$original] = format_date($time, 'medium', '', NULL, $language_code);
          break;
      }
    }
  }

  return $replacements;
}