<?php
// $Id:$

/**
 * @file
 * Insert support for mediareference fields.
 */

/**
 * Implementation of hook_insert_widgets().
 */
function mediareference_insert_widgets() {
  return array(
    'mediareference_widget' => array(
      'wrapper' => '.mediareference-item',
      'fields' => array(
        'alt' => 'input[name$="[alt]"], textarea[name$="[alt]"]',
        'title' => 'input[name$="[title]"], textarea[name$="[title]"]',
        'description' => 'input[name$="[description]"], textarea[name$="[description]"]',
      ),
    ),
  );
}


/**
 * Implementation of hook_insert_styles().
 */
function mediareference_insert_styles() {
  $insert_styles = array();

  $insert_styles['mediaref_image'] = array(
    'label' => t('Mediareference image'),
    'weight' => -10,
  );

  return $insert_styles;
}

/**
 * Implementation of hook_insert_content().
 */
function mediareference_insert_content($item, $style, $widget) {
  return theme('insert_image', $item, $widget);
}