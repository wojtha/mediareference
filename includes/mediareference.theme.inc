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
  $prefix = $suffix = '';

  $type = 'empty';
  if (!empty($element['type']) && !empty($element['type']['#value'])) {
    $type = $element['type']['#value'];
  }

  $prefix = '<div class="mediareference-item mediareference-'. $type .'-item">';
  $suffix = '</div>';

  return $prefix . theme('form_element', $element, $element['#children']) . $suffix;
}

/**
 *  Theme function for default mediareference field formatter
 */
function theme_mediareference_formatter_default($element = NULL) {

  if (empty($element['#item'])) {
    return '';
  }

  // PREPROCESS
  $item = $element['#item'];

  //dpm($item, 'formatter_default');
  $nid = $item['safe_nid'];
  $type = $item['type'];
  $title = isset($item['safe_title']) ? $item['safe_title'] : '';
  $description = isset($item['safe_description']) ? $item['safe_description'] : '';
  $link = isset($item['safe_link']) ? $item['safe_link'] : '';
  $color = isset($item['safe_color']) ? $item['safe_color'] : '';
  $thumbnail = '';
  if (isset($item['imagecache_preset']) && isset($item['filepath'])) {
    $thumbnail = theme('imagecache', $item['imagecache_preset'], $item['filepath']);
  }


  // RENDER
  $output = '';

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
