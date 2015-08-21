<?php

/**
 * @file trade rules functions
 */

/**
 * Implementation of hook_rules_event_info.
 */
function trade_rules_event_info() {
  // Let rules know about the node clone event.
  $items = array(
    'trade_order_create' => array(
      'label' => t('After create order'),
      'group' => t('Trade'),
      'variables' => array(
        'order' => array('type' => 'order', 'label' => t('The trade order'))
      ),
    ),
    'trade_order_update' => array(
      'label' => t('After update order'),
      'group' => t('Trade'),
      'variables' => array(
        'order' => array('type' => 'order', 'label' => t('The trade order'))
      ),
    ),
    'trade_order_delete' => array(
      'label' => t('After delete order'),
      'group' => t('Trade'),
      'variables' => array(
        'order' => array('type' => 'order', 'label' => t('The trade order'))
      ),
    ),
  );
  return $items;
}