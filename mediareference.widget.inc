<?php

/**
 * @file
 * CCK widget hook implementations for mediareference widget
 */

//=========================================
//    CCK WIDGET
//=========================================

/**
 * Implementation of FAPI hook_elements()
 */
function mediareference_elements() {

  $elements = array(
    'mediareference_widget' => array(
      '#input' => TRUE,
      '#delta' => 0,
      '#process' => array('mediareference_widget_process'),
      '#autocomplete_path' => FALSE,
    )
  );

  return $elements;
}

/**
 * Implementation of CCK hook_widget_info()
 */
function mediareference_widget_info() {
  return array(
    'mediareference_widget' => array(
      'label' => t('Mediareference'),
      'field types' => array('mediareference'),
      'multiple values' => CONTENT_HANDLE_CORE,
      'callbacks' => array('default value' => CONTENT_CALLBACK_CUSTOM),
      'description' => t('An edit widget for media nodereference fields that allows upload/preview of the media content and its description.' ),
    ),
  );
}

/**
 * Implementation of hook_widget()
 */
function mediareference_widget(&$form, &$form_state, $field, $items, $delta = 0) {
  // Value callback taken from nodereference_widget - type: nodereference_autocomplete.
  $element = array(
    '#type' => $field['widget']['type'],
    '#default_value' => isset($items[$delta]) ? $items[$delta] : NULL,
    '#after_build' => array('mediareference_widget_after_build'),
  );

  return $element;
}

/**
 * After build FAPI form builder
 *
 * We use after build handler because it is load always, even when form is loaded from cache.
 */
function mediareference_widget_after_build($form, &$form_state) {
  static $added = FALSE;

  if (!$added) {
    dialog_add_js();
    $path = drupal_get_path('module', 'mediareference');
    drupal_add_css($path .'/mediareference.css');
    drupal_add_js($path .'/mediareference.js');

    $added = TRUE;
  }

  return $form;
}

/**
 * Implementation of hook_widget_settings()
 */
function mediareference_widget_settings($op, $widget) {
  switch ($op) {
    case 'form':
      $form = array();

      $rows = (isset($widget['rows']) && is_numeric($widget['rows'])) ? $widget['rows'] : 5;
      $form['rows'] = array(
        '#type' => 'textfield',
        '#title' => t('Number of rows in Description field'),
        '#default_value' => $rows,
        '#element_validate' => array('_text_widget_settings_row_validate'),
        '#required' => TRUE,
        '#weight' => 8,
      );

      $cols = (isset($widget['cols']) && is_numeric($widget['cols'])) ? $widget['cols'] : 40;
      $form['cols'] = array(
        '#type' => 'textfield',
        '#title' => t('Number of columns in Description field'),
        '#default_value' => $cols,
        '#element_validate' => array('_text_widget_settings_row_validate'),
        '#required' => TRUE,
        '#weight' => 9,
      );
      return array_merge($form, nodereference_widget_settings('form', $widget));

    case 'save':
      $settings = array();
      $settings[] = 'rows';
      $settings[] = 'cols';
      return array_merge($settings, nodereference_widget_settings('save', $widget));
  }
}

/**
 * Process of our FAPI elements/widget
 */
function mediareference_widget_process($element, $edit, &$form_state, $form) {

  $defaults = $element['#default_value'];
  $field = content_fields($element['#field_name'], $element['#type_name']);

  $preview = '';

  if (empty($defaults['nid'])) {
    $preview = 'none <em>(use Browse button to attach some media)</em>';
    $element['#prefix'] = '<div class="mediareference empty">';
    $element['#suffix'] = '</div>';
  }
  else {

    if (empty($defaults['thumbnail']) || empty($defaults['title']) || empty($defaults['type'])) {
      $node = node_load(array('nid' => $defaults['nid']));
      $defaults['type'] = !empty($defaults['type']) ? $defaults['type'] : $node->type;
      $defaults['title'] = !empty($defaults['title']) ? $defaults['title'] : $node->title;
    }

    $type = $defaults['type'];
    $definition = '';
    $thumb_field = '';
    switch ($type) {
      case MEDIAREF_IMAGE_TYPE:
        $definition = MEDIAREF_IMAGE_FIELDS;
        $thumb_field = MEDIAREF_IMAGE_THUMBNAIL;
        break;
      case MEDIAREF_VIDEO_TYPE:
        $definition = MEDIAREF_VIDEO_FIELDS;
        $thumb_field = MEDIAREF_VIDEO_THUMBNAIL;
        break;
      case MEDIAREF_SLIDESHOW_TYPE:
        $definition = MEDIAREF_SLIDESHOW_FIELDS;
        $thumb_field = MEDIAREF_SLIDESHOW_THUMBNAIL;
        break;
    }

    if (empty($defaults['thumbnail'])) {
      $defaults['thumbnail'] = $node->{$thumb_field}[0]['filepath'];
    }
    $preview = theme('imagecache', MEDIAREF_IMAGECACHE_THUMB_PROFILE, $defaults['thumbnail']);

    $element['#prefix'] = '<div class="mediareference '. $type .'">';
    $element['#suffix'] = '</div>';

    $element['type'] = array(
      '#type' => 'hidden',
      '#value' => $defaults['type'],
    );

    $element['thumbnail'] = array(
      '#type' => 'hidden',
      '#value' => $defaults['thumbnail'],
    );

    $element['nid'] = array(
      '#type' => 'hidden',
      '#value' => $defaults['nid'],
    );

    if (strpos($definition, 'title') !== FALSE) {
      $element['title'] = array(
        '#title' => t('Title'),
        '#type' => 'textfield',
        '#default_value' => $defaults['title'],
        '#weight' => 1,
      );
    }

    if (strpos($definition, 'link') !== FALSE) {
      $element['link'] = array(
        '#title' => t('Link'),
        '#type' => 'textfield',
        '#default_value' => $defaults['link'],
        '#weight' => 2,
      );
    }

    if (strpos($definition, 'colour') !== FALSE) {
      $element['colour'] = array(
        '#title' => t('Color'),
        '#type' => 'colorpicker_textfield',
        '#default_value' => $defaults['colour'],
        '#weight' => 3,
      );
    }

    if (strpos($definition, 'description') !== FALSE) {
      $element['description'] = array(
        '#title' => t('Description'),
        '#type' => 'textarea',
        '#rows' => $field['widget']['rows'],
        '#cols' => $field['widget']['cols'],
        '#default_value' => $defaults['description'],
        '#weight' => 4,
      );

      if (! empty($field['description_text_processing'])) {
        $filter = isset($defaults['format']) ? $defaults['format'] : FILTER_FORMAT_DEFAULT;
        $parents = array_merge($element['#parents'], array('format'));
        $element['format'] = filter_form($filter, 5, $parents);
      }
    }
  }

  if ($type) {
    $element['type_info'] = array(
      '#type' => 'item',
      '#title' => t('Type'),
      '#value' => $type,
      '#weight' => -2,
    );
  }
  $element['preview'] = array(
    '#type' => 'item',
    '#title' => t('Preview'),
    '#value' => $preview,
    '#weight' => -1,
  );

  $element['actions'] = array(
    '#weight' => 6,
  );

  $field_name_css = str_replace('_', '-', $element['#field_name']);

  $element['actions']['remove'] = array(
    '#type' => 'button',
    '#value' => t('Remove'),
    '#ahah' => array(
      'path' => 'mediareference_js/'. $element['#type_name'] .'/'. $element['#field_name'] .'/'. $element['#delta'] . '/remove',
      'wrapper' => $field_name_css .'-items',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  $element['actions']['browse'] = array(
    '#type' => 'button',
    '#value' => t('Browse'),
    '#ahah' => array(
      'path' => 'mediareference_js/'. $element['#type_name'] .'/'. $element['#field_name'] .'/'. $element['#delta'] . '/update',
      'wrapper' => $field_name_css .'-items',
      'method' => 'replace',
      'effect' => 'fade',
    ),
    '#attributes' => array('class' => 'mrf-browse-button'),
  );

  // dpm($element);

  return $element;
}

//=====================================
//   MENU CALLBACKS
//=====================================

/**
 * AHAH menu callback
 *
 * @param string_type $type_name
 * @param string $field_name
 * @param int $delta_action
 * @param string $action
 */
function mediareference_widget_js($type_name, $field_name, $delta_action, $action) {

  $field = content_fields($field_name, $type_name);

  // Add the new elements to the stored form. Without adding the element
  // to the form, Drupal is not aware of this new elements existence and
  // will not process it. We retreive the cached form, add the element,
  // and resave.
  $form_build_id = $_POST['form_build_id'];
  $form_state = array('submitted' => FALSE);
  $form = form_get_cache($form_build_id, $form_state);

  // The form that we get from the cache is unbuilt. We need to build it so that
  // _value callbacks can be executed and $form_state['values'] populated.
  // We only want to affect $form_state['values'], not the $form itself
  // (built forms aren't supposed to enter the cache) nor the rest of $form_data,
  // so we use copies of $form and $form_data.
  $form_copy = $form;
  $form_state_copy = $form_state;
  $form_copy['#post'] = array();
  form_builder($_POST['form_id'], $form_copy, $form_state_copy);
  // Just grab the data we need.
  $form_state['values'] = $form_state_copy['values'];
  unset($form_copy, $form_state_copy);
  // Reset cached ids, so that they don't affect the actual form we output.
  form_clean_id(NULL, TRUE);

  // Sort the $form_state['values'] we just built *and* the incoming $_POST data
  // according to d-n-d reordering.
  $form_state['values'][$field_name] = mediareference_filter_children($form[$field_name]);
  foreach ($_POST[$field_name] as $delta => $item) {
    $form_state['values'][$field_name][$delta]['_weight'] = $item['_weight'];
  }
  $form_state['values'][$field_name] = _content_sort_items($field, $form_state['values'][$field_name]);
  $_POST[$field_name] = _content_sort_items($field, $_POST[$field_name]);

  if ($action == 'remove') {
    unset($form[$field_name][$delta_action]);
    unset($form_state['values'][$field_name][$delta_action]);
    unset($_POST[$field_name][$delta_action]);

    // reindex form ie. 1,2,4 => 1,2,3
    $items = mediareference_filter_children($form[$field_name]);
    $new_key = 0;
    $bak = array();
    foreach ($items as $old_key => $item) {
      if ($old_key != $new_key) {
        $bak = $form[$field_name][$old_key];
        unset($form[$field_name][$old_key]);
        $form[$field_name][$new_key] = $bak;

        $bak = $form_state['values'][$field_name][$old_key];
        unset($form_state['values'][$field_name][$old_key]);
        $form_state['values'][$field_name][$new_key] = $bak;
      }
      $new_key++;
    }
    unset($bak);

    if (count($items) == 0) {
      module_load_include('inc', 'content', 'includes/content.node_form');
      // Build our new form element for the whole field, asking for one more element.
      $form_state['item_count'] = array($field_name => 0);
      $form_element = content_field_form($form, $form_state, $field);
      $form[$field_name][0] = $form_element[$field_name][0];
    }
  }

  if ($action == 'update') {
    if (isset($_SESSION['lightframe']) && isset($_SESSION['lightframe']['nid']) && ($nid = intval($_SESSION['lightframe']['nid']))) {
      $node = node_load(array('nid' => $nid));
      if ($node) {
        $form_state['values'][$field_name][$delta_action]['nid'] = $nid;
        $form_state['values'][$field_name][$delta_action]['type'] = $node->type;
        $form_state['values'][$field_name][$delta_action]['title'] = $node->title;
        $_POST[$field_name][$delta] = $form_state['values'][$field_name][$delta_action];

        module_load_include('inc', 'content', 'includes/content.node_form');
        $form_element = content_field_form($form, $form_state, $field);
        // Let other modules alter it.
        drupal_alter('form', $form_element, array(), 'mediareference_widget_js');
        $form[$field_name][$delta_action] = $form_element[$field_name][$delta_action];
      }
    }
  }

  // Save rebuilded (but unprocessed!) form back to form cache.
  form_set_cache($form_build_id, $form, $form_state);

  // Rebuild the form, filter only needed elements and render the new output
  $form += array(
    '#post' => $_POST,
    '#programmed' => FALSE,
  );
  $form = form_builder($_POST['form_id'], $form, $form_state);
  $field_form = $form[$field_name];
  unset($field_form['#prefix'], $field_form['#suffix']);

  // If a newly inserted widget contains AHAH behaviors, they normally won't
  // work because AHAH doesn't know about those - it just attaches to the exact
  // form elements that were initially specified in the Drupal.settings object.
  // The new ones didn't exist then, so we need to update Drupal.settings
  // by ourselves in order to let AHAH know about those new form elements.
  $javascript = drupal_add_js(NULL, NULL);
  $output_js = isset($javascript['setting']) ? '<script type="text/javascript">jQuery.extend(Drupal.settings, '. drupal_to_js(call_user_func_array('array_merge_recursive', $javascript['setting'])) .');</script>' : '';

  $output = theme('status_messages');
  $output .= drupal_render($field_form) . $output_js;

  // Using drupal_json() breaks filefield's file upload, because the jQuery
  // Form plugin handles file uploads in a way that is not compatible with
  // 'text/javascript' response type.
  $GLOBALS['devel_shutdown'] =  FALSE;
  print drupal_to_js(array('status' => TRUE, 'data' => $output));
  exit;
}