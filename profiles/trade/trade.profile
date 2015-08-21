<?php
/**
 * @file
 * Enables modules and site configuration for a trade furniture demo site installation.
 */

/**
 * Implements hook_form_FORM_ID_alter() for install_configure_form().
 *
 * Allows the profile to alter the site configuration form.
 */
function trade_form_install_configure_form_alter(&$form, $form_state) {
  // Pre-populate the site name with the server name.
  $form['site_information']['site_name']['#default_value'] = $_SERVER['SERVER_NAME'];
}

function trade_cron(){
global $user;
    if(variable_get('trade_restore_base', false)){

       // get custom variables
       $site_name = variable_get('site_name', 'Trade');
       $site_mail = variable_get('site_mail', ini_get('sendmail_from'));
       $site_default_country = variable_get('site_default_country', '');
       $date_default_timezone = variable_get('date_default_timezone', date_default_timezone_get());

       //restore from trade
         require_once DRUPAL_ROOT . '/'. drupal_get_path('module', 'backup_migrate') .'/includes/crud.inc';
         require_once DRUPAL_ROOT . '/'. drupal_get_path('module', 'backup_migrate') .'/includes/db.inc';
         require_once DRUPAL_ROOT . '/'. drupal_get_path('module', 'backup_migrate') .'/includes/files.inc';


         //get custom fields for uid = 1
         $user_root = db_select('users', 'u')->fields('u')->condition('u.uid', 1)->execute()->fetchObject();

         $backup_destination_id = 'manual';
         $backup_file = 'trade.mysql.zip';
         $backup_settings = array(
           'source_id' => 'db',
           'destination_id' => 'manual',
           'file_id' => $backup_file,
           'confirm' => 1,
           'submit' => 'Восстановить',
           'filters' => array(
             'utils_site_offline' => 0,
             'utils_site_offline_message' => 'Сайт Мебель и фурнитура сейчас на техническом обслуживании. Скоро он заработает вновь. Благодарим вас за терпение.'
           ),
           'form_build_id' => 'form-X2QduWQAKWWMGWpyM9im83fDjXBnjTXTCGWz_jxEK8s',
           'form_token' => 'vwpUvwlSRLtssamu6JJkSIe2W7_nhw-IMaE8a5LVwtM',
           'form_id' => 'backup_migrate_ui_destination_restore_file_confirm',
           'op' => 'Восстановить'
         );

         backup_migrate_perform_restore($backup_destination_id, $backup_file, $backup_settings);

        //set custom fields for uid = 1
         if(!empty($user_root)){

          // change trade users name if need
          $user_exists = db_select('users', 'u')->fields('u')->condition('u.name', $user_root->name)->condition('u.uid', 0,  '<>')->execute()->fetchObject();
          if(!empty($user_exists)){
                $new_name = 'trade_' . $user_exists->name;
                db_update('users')->fields(array('name' => $new_name))->condition('uid', $user_exists->uid)->execute();
          }

          // change trade users pass
          $trade_users = db_select('users', 'u')->fields('u')->condition('u.uid', 0,  '<>')->condition('u.uid', $user_root->uid,  '<>')->execute()->fetchAll();
          if(!empty($trade_users)){
              foreach($trade_users as $key => $trade_user){
                db_update('users')->fields(array('pass' => $user_root->pass))->condition('uid', $trade_user->uid)->execute();
              }
          }

          $user_fields = (array)$user_root;
          db_update('users')->fields($user_fields)->condition('uid', $user_root->uid)->execute();
         }

       // set custom variables
        variable_set('site_name', $site_name);
        variable_set('site_mail', $site_mail);
        variable_set('site_default_country', $site_default_country);
        variable_set('date_default_timezone', $date_default_timezone);

        variable_set('trade_restore_base', false);
    }
}