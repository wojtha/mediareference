<?php

/**
 * @file
 * Theme functions for mediareference widget
 */

//=====================================
//   THEME FUNCTIONS
//=====================================

/**
 * Theme function for mediareference widget
 */
function theme_mediareference_widget($element) {
  return theme('form_element', $element, $element['#children']);
}

/**
 *  Theme function for default mediareference field formatter
 */
function theme_mediareference_formatter_default($element = NULL) {
  $output = '';

  if (empty($element['#item'])) {
    return $output;
  }

  $data = $element['#item'];
  //dpm($data, 'formatter_default');

  // TODO: rethink this
  $field = content_fields($element['#field_name'], $element['#type_name']);
  $nid = $data['safe_nid'];
  $node = node_load(array('nid' => $nid));
  $type = $node->type;
  $thumbnail = '';

  if ($thumb_field = $field['widget']['thumbnail_type_fields'][$type]) {
    $filepath = $node->{$thumb_field}[0]['filepath'];
    if ($imagecache_preset = imagecache_preset($field['widget']['thumbnail_imagecache_preset'])) {
      $thumbnail = theme('imagecache', $imagecache_preset['presetname'], $filepath);
    }
  }

  $title = isset($data['safe_title']) ? $data['safe_title'] : '';
  $description = isset($data['safe_description']) ? $data['safe_description'] : '';
  $link = isset($data['safe_link']) ? $data['safe_link'] : '';
  $color = isset($data['safe_color']) ? $data['safe_color'] : '';

  $output .= '<div class="mediareference-item '. $type .'">';

  if ($link) {
    $output .= '<a href="'. $link .'"">'. $thumbnail .'</a>';
    $output .= '<h3><a href="'. $link .'">'. $title .'</a></h3>';
    $output .= '<a href="'. $link .'">'. $description .'</a>';
  }
  else {
    $output .= $thumbnail;
    $output .= '<h3>'. $title .'</h3>';
    $output .= $description;
  }
  $output .= '</div>';

  return $output;
}