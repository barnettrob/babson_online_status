<?php

/**
 * Get online the online bubble
 * filled with the color according to the user's online status.
 */
function babson_online_status_bubble_fill($uname) {
  // Get colors representing available, unavailable, and invisible for bubble.
  $online_status_colors = babson_online_status_colors();

  if (!empty($uname)) {
    //Check if user is online.
    $online_status = babson_online_status_user_check($uname);
    switch($online_status) {
      case 1:
        // Available.
        $bubble_class = $online_status_colors['available'];;
        break;
      case 2:
        // Unavailable.
        $bubble_class = $online_status_colors['unavailable'];
        break;
      case 3:
        // Offline/Invisible.
        $bubble_class = $online_status_colors['invisible'];
        break;
    }
  }
  else {
    $bubble_class = $online_status_colors['unavailable'];
  }

  return babson_online_status_svg($bubble_class);
}

/**
 * Get the svg for the online status bubble.
 */
function babson_online_status_svg($status_class) {
  return '<svg height="16" width="16">
            <circle cx="8" cy="8" r="4" stroke="black" stroke-width="1" class="' . $status_class . '" />
         </svg> ';
}

/**
 * Online Status Select Box for contact list.
 */
function babson_online_status_selectbox() {
  global $user;
  drupal_add_css(drupal_get_path('module', 'babson_online_status') . '/css/babson_online_status.css');
  drupal_add_js(drupal_get_path('module', 'babson_online_status') . '/js/babson_online_status_selectbox.js', array('scope' => 'footer'));

  // Get status colors.
  $online_status_colors = babson_online_status_colors();
  $green = babson_online_status_svg($online_status_colors['available']);
  $red = babson_online_status_svg($online_status_colors['unavailable']);
  $gray = babson_online_status_svg($online_status_colors['invisible']);

  $select_down_arrow = '<svg class="svg-triangle">
                            <polygon points="0,0 10,0 5,10"/>
                        </svg>';

  /*** Connect to Bell database. ***/
  $connection = db_pdo_bell_switch();

  $status_set = $connection->prepare("SELECT online_status
                                      FROM babson_online_status
                                      WHERE name = :name");
  $status_set->execute(array(':name' => $user->name));
  $status_set = $status_set->fetchColumn();

  $selected1 = '';
  $selected2 = '';
  $selected3 = '';
  if (!empty($status_set)) {
    switch ($status_set) {
      case 1:
        $status_val = 'user-online-status-available';
        $selected1 = ' selected';
        $selected_text = t('I\'m Available');
        $color = $green;
        break;
      case 2:
        $status_val = 'user-online-status-unavailable';
        $selected2 = ' selected';
        $selected_text = t('I\'m Unavailable');
        $color = $red;
        break;
      case 3:
        $status_val = 'user-online-status-invisible';
        $selected3 = ' selected';
        $selected_text = t('I\'m Invisible');
        $color = $gray;
        break;
      default:
        $status_val = 'user-online-status-available';
        $selected_text = t('I\'m Available');
        $color = $green;
    }
  }
  else {
    $status_val = 'user-online-status-available';
    $selected_text = t('I\'m Available');
    $color = $green;
  }

  $username = check_plain($user->name);

  // @todo: Convert this to phptemplate file.
  $online_status_select = '<div class="contact-list-online-status-select">';
  $online_status_select .=    '<div class="contact-list-online-status-wrapper">';
  $online_status_select .=      '<div class="status-label-wrapper">';
  $online_status_select .=        '<label for="user-status-select" class="list-section-label">Status:</label>';
  $online_status_select .=      '</div>';
  $online_status_select .=      '<div id="contact-list-user-status-select-wrapper">';
  $online_status_select .=        '<span class="selected">' . $color .  $selected_text . '</span>' . $select_down_arrow;
  $online_status_select .=        '<input type="hidden" name="subject" value="' . $status_val . '" />';
  $online_status_select .=        '<ul id="contact-list-select-user-select">';
  $online_status_select .=          '<li id="user-online-status-available" class="' . $username . $selected1 . '">' . $green . 'I\'m Available</li>';
  $online_status_select .=          '<li id="user-online-status-unavailable" class="' . $username . $selected2 . '">' . $red . 'I\'m Unavailable</li>';
  $online_status_select .=          '<li id="user-online-status-invisible" class="' . $username . $selected3 . '">' . $gray . 'I\'m Invisible</li>';
  $online_status_select .=        '</ul>';
  $online_status_select .=      '</div>'; // End #contact-list-user-status-select.
  $online_status_select .=    '</div>';
  $online_status_select .= '</div>';

  return $online_status_select;
}

/**
 * Get all users currently online.
 */
function babson_online_status_users_online() {
  global $user;
  // Count users active within the defined period.
  $interval = REQUEST_TIME - variable_get('user_block_seconds_online', 900);

  $online_users_sql = "SELECT u.name, u.access, IFNULL(os.online_status, 1) AS online_status
          FROM users u
          LEFT JOIN babson_online_status os
          ON u.name = os.name
          WHERE u.access > :interval
          AND u.uid > 0";

  /*** Connect to Bell database. ***/
  $connection = db_pdo_bell_switch();

  $online_users_results = $connection->prepare($online_users_sql);
  $online_users_results->execute(array(':interval' => $interval));
  $online_users_results = $online_users_results->fetchAll();

  $online_users = array();

  foreach ($online_users_results as $row) {
    //$online_users[$row->uid] = TRUE;
    $online_users[$row['name']] = $row['online_status'];
  }

  // Add current user if not yet in $online_users_results.
  if (!array_key_exists($user->name, $online_users)) {
    // Get current user status.
    $curr_user_status = $connection->prepare('SELECT online_status
                                              FROM babson_online_status
                                              WHERE name = :name');
    $curr_user_status->execute(array(':name' => $user->name));
    $curr_user_status = $curr_user_status->fetchColumn();
    // If no record then set status to available (1).
    $curr_user_status = $curr_user_status == FALSE ? 1 : $curr_user_status;
    // Add current user to $online_users array.
    $online_users[$user->name] = $curr_user_status;
  }
  return $online_users;
}

/**
 * Get online status for user.
 *
 * Based on online block in core user module.
 */
function babson_online_status_user_check($uname) {
  /** Status key:
   1 = Available
   2 = Unavailable
   3 = Offline/Invisible
   **/
  $status = 3;

  $online_users = babson_online_status_users_online();
  if (isset($online_users[$uname])) {
    switch ($online_users[$uname]) {
      case 1:
        $status = 1;
        break;
      case 2:
        $status = 2;
        break;
      case 3:
        $status = 3;
        break;
    }
  }

  return $status;
}

/**
 * Colors for online status.
 */
function babson_online_status_colors() {
  $available = 'babson-status-available';
  $unavailable = 'babson-status-unavailable';
  $invisible = 'babson-status-invisible';

  $status_classes = array('available' => $available, 'unavailable' => $unavailable, 'invisible' => $invisible);

  return $status_classes;
}

/**
 * Ajax call to set status from contact list status select box.
 */
function babson_online_status_ajax_contact_list_set_status() {
  $name = $_POST['username'];
  $status_set = $_POST['status'];

  switch ($status_set) {
    case 'available':
      $status = 1;
      break;
    case 'unavailable':
      $status = 2;
      break;
    case 'invisible':
      $status = 3;
      break;
  }

  //Checks if key exists. If so then updates, otherwise inserts.
  try {
    $online_status_rec = "SELECT COUNT(*) FROM babson_online_status WHERE name = :name";
        /*** Connect to Bell database. ***/
    $connection = db_pdo_bell_switch();

    $online_status_rec_results = $connection->prepare($online_status_rec);
    $online_status_rec_results->execute(array(':name' => $name));
    $online_status_rec_results = $online_status_rec_results->fetchColumn();

    if ($online_status_rec_results == 0) {
      $online_status_rec_insert = $connection->prepare("INSERT INTO babson_online_status (name, online_status) VALUES (:name, :status)");
      if ($online_status_rec_insert->execute(array(
        ':name' => $name,
        ':status' => $status
      ))) {
        $success = TRUE;
      }
      else {
        $success = FALSE;
      }
    }
    else {
      $online_status_rec_update = $connection->prepare("UPDATE babson_online_status
                             SET online_status = :status
                             WHERE name =:name");
      if ($online_status_rec_update->execute(array(
         ':status' => $status,
         ':name' => $name,
      ))) {
        $success = TRUE;
      }
      else {
        $success = FALSE;
      }
    }
  }
  catch (PDOException $e) {
    // Query failed.  Get message.
    $success = $e->getMessage();
    watchdog('set online status error', $success);
  }

  drupal_json_output(array(
    'success' => $success,
    'new_status' => $status_set,
  ));
}
