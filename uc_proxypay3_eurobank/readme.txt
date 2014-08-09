

For multilingual sites add the following code to settings.php (according to http://drupal.org/node/313272 )

$conf['i18n_variables'] = array(

  // uc_proxypay3_eurobank variables
  
  //eurobank interface
  'uc_proxypay3_eurobank_currency', //if you intend to use different currency for different language
  'uc_proxypay3_eurobank_language',
  
  //messages
  'uc_proxypay3_eurobank_method_title',
  'uc_proxypay3_eurobank_checkout_button',
  'uc_proxypay3_eurobank_card_not_authorised',
  'uc_proxypay3_eurobank_card_error',
  'uc_proxypay3_eurobank_card_pending',
  
  //only if you intend to edit the redirect variables
  'uc_proxypay3_eurobank_redirect_success',
  'uc_proxypay3_eurobank_redirect_error',
  
);


