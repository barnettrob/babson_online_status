(function ($) {

  Drupal.behaviors.babson_online_status_contactme = {
    attach: function(context, settings) {
      $('.presence-icon', context).click(function(event) {
        $(this).siblings('.contact-me').addClass('show');
      });

      $('.close-contact-me', context).click(function(event) {
        $(this).closest('.contact-me').removeClass('show');
      });
    }
  };
})(jQuery);