<?php

/**
 * @file
 * Hooks provided by the trade module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Respond to trade order deletion.
 *
 *
 * @param $order
 *   The $order that is being deleted.
 *
 */
function hook_trade_order_delete($order) {
  db_delete('mytable')
    ->condition('uid', $order->oid)
    ->execute();
}

/**
 * A trade order was created.
 *
 * The module should save its custom additions to the trade order into the
 * database.
 *
 * @param $order
 *   The object of form values submitted by the trade order.
 *
 * @see hook_trade_order_update()
 */
function hook_trade_order_create(&$order) {
  db_insert('mytable')
    ->fields(array(
      'myfield' => $order['myfield'],
      'uid' => $order->oid,
    ))
    ->execute();
}

/**
 * A trade order was updated.
 *
 * Modules may use this hook to update their trade order data in a custom storage
 * after a trade order has been updated.
 *
 * @param $order
 *   The object of form values submitted by the trade order.
 *
 * @see hook_trade_order_insert()
 */
function hook_trade_order_update(&$order) {
  db_insert('mytable')
    ->fields(array(
      'oid' => $order->uid,
      'changed' => time(),
    ))
    ->execute();
}