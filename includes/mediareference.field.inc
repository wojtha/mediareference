<?php


/**
 * @file
 * CCK field hook implementations for mediareference field
 */

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
      $columns['fid'] = array('type' => 'int', 'unsigned' => TRUE, 'not null' => FALSE, 'index' => TRUE);
      $columns['title'] = array('type' => 'varchar', 'length' => 255, 'not null' => FALSE, 'sortable' => TRUE, 'default' => '');
      $columns['description'] = array('type' => 'text', 'not null' => FALSE, 'sortable' => FALSE);
      $columns['link'] = array('type' => 'varchar', 'length' => 255, 'not null' => FALSE, 'sortable' => FALSE, 'default' => '');
      $columns['colour'] = array('type' => 'varchar', 'length' => 7, 'not null' => FALSE, 'sortable' => FALSE, 'default' => '');
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

      foreach (array('title', 'description', 'link', 'colour') as $type) {
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

      return $form;

    case 'save':
      $settings = nodereference_field_settings('save', $field);
      $settings[] = 'mediareference_fields';
      $settings[] = 'description_text_processing';

      return $settings;

     case 'views data':
      return nodereference_field_settings('views data', $field);
  }
}

/**
 * Implementation of CCK hook_field()
 */
function mediareference_field($op, $node, $field, &$items, $teaser, $page) {
  switch ($op) {
    case 'load':
      module_load_include('inc', 'filefield', 'field_file');
      foreach ($items as $delta => &$item) {
        if ($file = field_file_load($item['fid'])) {
          $item['filepath'] = $file['filepath'];
        }
      }
      break;

    case 'sanitize':
      foreach ($items as $delta => $item) {
        $item_keys = array_keys($item);
        $allowed_keys = array_merge(array('nid', 'fid', 'filepath'), array_filter($field['mediareference_fields']));
        // FIX: from PHP >= 5.1.0 we can do array_intersect_key() directly
        $item_keys = (array_intersect($item_keys, $allowed_keys));
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
      }
    break;

    case 'validate':
      if (is_array($items)) {
        foreach ($items as $delta => $item) {
          if ($item['link'] != '' && !valid_url(trim($item['link']))) {
            form_set_error($field['field_name'],t('Link "%url" is not a valid link.', array('%url' => $item['link'])));
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