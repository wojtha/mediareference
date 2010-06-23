<?php


/**
 * @file
 * CCK field hook implementations for mediareference field
 */


function mediareference_field_subfields($type = NULL) {
  static $subfields = NULL;

  // initialize
  if (is_null($subfields)) {
    $subfields = array();
    $subfields['title'] = array();
    $subfields['description'] = array();
    $subfields['link'] = array();
    $subfields['colour'] = array();
    $subfields['slide_animation'] = array(
      'data' => TRUE,
    );
    $subfields['slide_delay'] = array(
      'data' => TRUE,
    );

    // load defaults
    $defaults = array(
      'data' => FALSE,
    );
    foreach ($subfields as $id => $subfield) {
    	$subfields[$id] = array_merge($defaults, $subfield);
    }
  }

  if ($type === 'names') {
    return array_keys($subfields);
  }

  return $subfields;
}

//=========================================
//    CCK FIELD HOOKS
//=========================================

/**
 * Implementation of CCK hook_field_settings()
 */
function mediareference_field_info() {
  return array(
    'mediareference' => array(
      'label' => t('Media reference'),
      'description' => t('Stores an nodereference, custom title, description, link and color style.'),
    )
  );
}

/**
 * Implementation of CCK hook_field_settings()
 */
function mediareference_field_settings($op, $field) {
  switch($op) {
    case 'database columns':
      $columns = array();
      $columns += nodereference_field_settings('database columns', $field);
      $columns['fid'] = array('type' => 'int', 'unsigned' => TRUE, 'not null' => FALSE, 'index' => TRUE, 'views' => TRUE);
      $columns['title'] = array('type' => 'varchar', 'length' => 255, 'not null' => FALSE, 'sortable' => TRUE, 'default' => '', 'views' => TRUE);
      $columns['description'] = array('type' => 'text', 'not null' => FALSE, 'sortable' => FALSE, 'views' => TRUE);
      $columns['link'] = array('type' => 'varchar', 'length' => 255, 'not null' => FALSE, 'sortable' => FALSE, 'default' => '', 'views' => TRUE);
      $columns['colour'] = array('type' => 'varchar', 'length' => 7, 'not null' => FALSE, 'sortable' => FALSE, 'default' => '', 'views' => TRUE);
      $columns['data'] = array('type' => 'text', 'serialize' => TRUE, 'views' => TRUE);
      return $columns;

    case 'form':
      $form = array();
      $form += nodereference_field_settings('form', $field);
      $form['description_text_processing'] = array(
        '#type' => 'radios',
        '#title' => t('Text processing for Description'),
        '#default_value' => is_numeric($field['description_text_processing']) ? $field['description_text_processing'] : 0,
        '#options' => array(
            0 => t('Plain text'),
            1 => t('Filtered text (user selects input format)')
          ),
      );
      $form['mediareference_fields'] = array(
        '#title' => t('Mediareference Fields'),
        '#description' => t('Specify which fields will be enabled.'),
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#tree' => TRUE,
      );

      $subfields = mediareference_field_subfields('names');
      foreach ($subfields as $type) {
        $form['mediareference_fields'][$type] = array(
          '#type' => 'radios',
          '#title' => t('Field %type', array('%type' => drupal_ucfirst($type))),
          '#default_value' => !empty($field['mediareference_fields'][$type]) ? $field['mediareference_fields'][$type] : 0,
          '#options' => array(
            MEDIAREF_FIELD_DISABLED => t('Disabled'),
            MEDIAREF_FIELD_OPTIONAL => t('Optional'),
            MEDIAREF_FIELD_REQUIRED => t('Required'),
          ),
        );
      }

      $form['mediareference_formatters'] = array(
        '#title' => t('Formatters Settings'),
        '#type' => 'fieldset',
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
        '#tree' => TRUE,
      );

      $form['mediareference_formatters']['slideshow'] = array(
        '#title' => t('Mediareference Slideshow Formatter'),
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#tree' => TRUE,
      );

      $form['mediareference_formatters']['slideshow']['slide_delay_default'] = array(
        '#title' => t('Default slide delay'),
        '#type' => 'textfield',
        '#size' => 4,
        '#maxlength' => 4,
        '#field_suffix' => 's',
        '#default_value' => $field['mediareference_slideshow']['slide_delay_default'] ? $field['mediareference_slideshow']['slide_delay_default'] : 10,
        '#description' => t('Default delay between automatically moving to the next.'),
      );

      return $form;

    case 'save':
      $settings = nodereference_field_settings('save', $field);
      $settings[] = 'mediareference_fields';
      $settings[] = 'description_text_processing';
      $settings[] = 'mediareference_formatters';

      return $settings;

     case 'views data':
       // TODO: look at filefield for inspiration
      return nodereference_field_settings('views data', $field);
  }
}

/**
 * Implementation of CCK hook_field()
 */
function mediareference_field($op, $node, $field, &$items, $teaser, $page) {
//  dpm('mediareference_field() $op=' . $op);
  switch ($op) {
    case 'load':
      module_load_include('inc', 'filefield', 'field_file');

      if (empty($items)) {
        return array();
      }

      foreach ($items as $delta => &$item) {
        // Despite hook_content_is_empty(), CCK still doesn't filter out
        // empty items from $op = 'load', so we need to do that ourselves.
        if (empty($item['nid']) || empty($item['fid']) || !($file = field_file_load($item['fid']))) {
          $items[$delta] = NULL;
        }
        else {
          $item['data'] = unserialize($item['data']);
          if ($item['data'] === FALSE) {
            $item['data'] = array();
          }

          // unserialize 'data'
          mediareference_field_unserialize($item);

          $item['data']['filepath'] = $file['filepath'];
          $item['imagecache_preset'] = imagecache_preset($field['widget']['thumbnail_imagecache_preset']);
        }
      }

      //dpm($items, 'mediareference_field($op = load)');
      break;

    case 'sanitize':
      foreach ($items as $delta => &$item) {

        // unserialize 'data'
        mediareference_field_unserialize($item);

        if (is_array($item['data'])) {
          $item = array_merge($item, $item['data']);
          unset($item['data']);
        }

        $item_keys = array_keys($item);
        $sanitized_keys = array_merge(array('nid', 'fid', 'filepath'), array_keys(array_filter($field['mediareference_fields'])));
        // FIX: from PHP >= 5.1.0 we can do array_intersect_key() directly
        $item_keys = (array_intersect($item_keys, $sanitized_keys));
        foreach ($item_keys as $key) {
          $data = $item[$key];
          // process description field
          if ($key === 'description' && !empty($field['description_text_processing'])) {
            $check_access = is_null($node) || (isset($node->build_mode) && $node->build_mode == NODE_BUILD_PREVIEW);
            $data = check_markup($data, $item['format'], $check_access);
          }
          else {
            $data = check_plain($data);
          }

          $items[$delta]['safe_' . $key] = $data;
        }

        // resource intensive?
        if (($file = field_file_load($item['fid'])) && ($preset = imagecache_preset($field['widget']['thumbnail_imagecache_preset']))) {
          $item['imagecache_preset'] = $preset['presetname'];
          $item['filepath'] = $file['filepath'];
        }
      }
    break;

    case 'validate':
      if (is_array($items)) {
        foreach ($items as $delta => $item) {
          if (!empty($item['link']) && !valid_url(trim($item['link']))) {
            form_set_error($field['field_name'], t('Link "%url" is not a valid link.', array('%url' => $item['link'])));
          }
        }
     }
     break;
  }

  // provide all operations also on nodereference field
  nodereference_field($op, $node, $field, $items, $teaser, $page);
}

/**
 * Implementation of hook_field_formatter_info()
 */
function mediareference_field_formatter_info() {
  return array(
    'default' => array(
      'label' => t('Title and description'),
      'field types' => array('mediareference'),
    ),
  );
}

/**
 * Implementation of CCK hook_content_is_empty()
 */
function mediareference_content_is_empty($item, $field) {
  return nodereference_content_is_empty($item, $field);
}


function mediareference_field_unserialize(&$item) {
  // Temporary fix to unserialize data serialized multiple times.
  // See the FileField issue http://drupal.org/node/402860.
  // And the CCK issue http://drupal.org/node/407446.
  while (! empty($item['data']) && is_string($item['data'])) {
    $item['data'] = unserialize($item['data']);
  }
}