<?php
// $Id$

/**
 * @file
 * Integrates proxypay for eurobank bank (Greece) redirected payment service.
 */


function uc_proxypay3_eurobank_confirm() {
  
  if ( variable_get('uc_proxypay3_eurobank_merchant_password', '') != $_POST['Password']) {
    print '[NOTOK]';
    watchdog('proxypay3_eurobank', 'Confirmation Error: Wrong password ' . $_POST['Password'] );
    exit();
  }
  $result = db_query("SELECT status FROM {uc_proxypay3_eurobank} WHERE order_id = %d ", check_plain($_POST['Ref']));
  $row = db_fetch_object($result);
  //watchdog('proxypay3_eurobank', 'Confirmation Arrived: ' . print_r($_REQUEST,TRUE) . print_r($row,TRUE) . ' - ' . check_plain($_POST['Ref']));
  if ($row && $row->status == 'Validated') {
    print '[OK]';

    watchdog('proxypay3_eurobank', 'Receiving new order notification for order !order_id.', array('!order_id' => check_plain($_POST['Ref'])));
    db_query("UPDATE {uc_proxypay3_eurobank} SET ". 
      "amount = '%s', curency = '%s', currencySymbol = '%s', trans_id = '%s', status = 'Confirmed', ".
      "var1 = '%s', var2 = '%s', var3 = '%s', var4 = '%s', var5 = '%s', var6 = '%s', var7 = '%s', var8 = '%s', var9 = '%s', ".
      "Method = '%s', DTime = '%s', RemoteAddr = '%s' WHERE order_id = %d ",
      $_POST['Amount'], $_POST['Currency'], $_POST['Currencysymbol'], $_POST['Transid'], 
      $_POST['Var1'], $_POST['Var2'], $_POST['Var3'], $_POST['Var4'], $_POST['Var5'], $_POST['Var6'], $_POST['Var7'], $_POST['Var8'], $_POST['Var9'], 
      $_POST['Method'], $_POST['DateTime'], $_POST['RemoteAddr'], $_POST['Ref']);

    $order = uc_order_load($_POST['Ref']);
    if ($order === FALSE ) {
      //error
      //print t('An error has occurred during payment.  Please contact us to ensure your order has submitted.');
      //exit();
    }
    uc_cart_complete_sale($order, variable_get('uc_new_customer_login', FALSE));
    uc_payment_enter($order->order_id, 'proxypay3_eurobank', $_POST['Amount'], 0, NULL, t('Paid by Credit Card (Eurobank)'));
    uc_order_comment_save($order->order_id, 0, t('Payment received.'), 'admin');
  }
}

function uc_proxypay3_eurobank_validate() {
  $myRef = $_POST['Ref'];
  if ($myRef =='') $myRef = 0;

  $myCurrency = '0' . trim($_POST['Currency']);
  $myStatusMessage = '';
  $myOutput = '[OK]';
  
  $result = db_query("SELECT * FROM {uc_proxypay3_eurobank} WHERE order_id = %d ", $myRef);

  if ($row = db_fetch_object($result)) {
    
    if ($myCurrency != trim($row->curency) ) {
      $myOutput =  '[BAD]';
      $myStatusMessage .= ' Wrong curency ' . $myCurrency . ' -';
    }
    if ($_POST['Amount']*1.0 != trim($row->amount)*1.0 ) {
      $myOutput =  '[BAD]';
      $myStatusMessage .= ' Wrong amount ' . $_POST['Amount'] . ' -';
    }
    if ($_POST['Shop'] != variable_get('uc_proxypay3_eurobank_merchantID', '')) {
      $myOutput =  '[BAD]';
      $myStatusMessage .= ' Wrong merchantID ' . $_POST['Shop'] . ' -';
    }
    if ($myOutput == '[BAD]')
      $myStatusMessage = 'Validation Error: ' . $myStatusMessage;
    else
      $myStatusMessage = 'Validated';

    db_query("UPDATE {uc_proxypay3_eurobank} set status = '%s' WHERE order_id = %d ", $myStatusMessage, $myRef);
  }
  else {
    $myOutput =  '[BAD]';
    $myStatusMessage .= ' Wrong OrderID -';
    watchdog('proxypay3_eurobank', 'Validation Error: Wrong Order ID ' . $myRef );
  }
  print $myOutput;
}

function uc_proxypay3_eurobank_error() {
  drupal_set_message(variable_get('uc_proxypay3_eurobank_card_not_authorised', ''));
  drupal_goto(variable_get('uc_proxypay3_eurobank_redirect_error', ''));
}

function uc_proxypay3_eurobank_complete() {

  $order = uc_order_load($_REQUEST['ref']);

  if ($order === FALSE ) {
    drupal_set_message( variable_get('uc_proxypay3_eurobank_card_error', ''));
    drupal_goto(variable_get('uc_proxypay3_eurobank_redirect_error', ''));
  }
  
  if (intval($_SESSION['cart_order']) != $order->order_id) {
    $_SESSION['cart_order'] = $order->order_id;
  }


  // This lets us know it's a legitimate access of the complete page.
  $result = db_query("SELECT status FROM {uc_proxypay3_eurobank} WHERE order_id = %d ", $order->order_id);
  if ($row = db_fetch_object($result))
    $status = $row->status;
  
  
  
  if ($status == 'Confirmed' || $status == 'Validated') {
    if ($status == 'Validated')
      drupal_set_message(variable_get('uc_proxypay3_eurobank_card_pending', ''));

    $_SESSION['do_complete'] = TRUE;
    drupal_goto(variable_get('uc_proxypay3_eurobank_redirect_success', ''));
  }
  else {
    drupal_set_message( variable_get('uc_proxypay3_eurobank_card_error', '') . $status);
    drupal_goto(variable_get('uc_proxypay3_eurobank_redirect_error', ''));
  }
  return ;
}