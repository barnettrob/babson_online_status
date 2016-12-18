/**
 * @file
 * Attaches javascript for online status for select box in contact list.
 */
(function($) {
  Drupal.behaviors.babson_online_status_select = {
    attach: function (context, settings) {
      /** Status div-only select box **/
      $('#contact-list-user-status-select-wrapper')
        .click(
        function() {
          if ($('#contact-list-select-user-select').hasClass('open')) {
            $('#contact-list-select-user-select').removeClass('open');
          }
          else {
            $('#contact-list-select-user-select').addClass('open');
          }
          if ($(this).hasClass('open')) {
            $(this).removeClass('open');
            return false;
          } else {
            $(this).addClass('open');
            return false;
          }

          return false;
        })
        .hover(
        function() {
        },
        function() {
          if ($('#contact-list-select-user-select').hasClass('open')) {
            $('#contact-list-select-user-select').removeClass('open');
          }
          else {

          }
          $(this).removeClass('open');
        });
      $('#contact-list-user-status-select-wrapper ul li')
        .click(
        function() {
          var sitem = $(this).html();
          var sid = $(this).attr('id');
          $(this).siblings('li').removeClass('selected');
          $(this).addClass('selected');
          // Ajax call to set online status in babson_online_status table.
          var status_set = sid.replace('user-online-status-', '');
          var username = $(this).attr('class').replace(' selected', '');
          $.ajax({
            type: "POST",
            url: '/babson-online-status/contact-list-select-set-status',
            data: {
              'status' :status_set,
              'username' :username
            },
            dataType: 'json',
            success: function(data) {
              if (data.success !== true) {
                alert('Status not saved.  Please contact ####@babson.edu.');
              }
              else {
                // Change the ribbon status for logged in user to reflect change.
                var svg = $('#ribbon .views-field-name .presence-icon');
                var user_bubble = $('.' + username).siblings('.presence-icon');
                switch (data.new_status) {
                  case 'available':
                    svg.removeClass('change-available change-unavailable change-invisible');
                    user_bubble.removeClass('make-available make-unavailable make-invisible');
                    svg.addClass('change-available');
                    user_bubble.addClass('make-available');
                    break;
                  case 'unavailable':
                    svg.removeClass('change-available change-unavailable change-invisible');
                    user_bubble.removeClass('make-available make-unavailable make-invisible');
                    svg.addClass('change-unavailable');
                    user_bubble.addClass('make-unavailable');
                    break;
                  case 'invisible':
                    svg.removeClass('change-available change-unavailable change-invisible');
                    user_bubble.removeClass('make-available make-unavailable make-invisible');
                    svg.addClass('change-invisible');
                    user_bubble.addClass('make-invisible');
                    break;
                }
              }
            }
          });
          $(this).parent('ul').siblings('span.selected').html(sitem);
          $(this).parent('ul').siblings('input').val(sid);
        });
    }
  }
}(jQuery));
