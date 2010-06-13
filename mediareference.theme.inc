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
function theme_mediareference_widget(&$element) {
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

  $nid = $data['safe_nid'];
  $node = node_load(array('nid' => $nid));
  $type = $node->type;
  $title = isset($data['safe_title']) ? $data['safe_title'] : '';
  $description = isset($data['safe_description']) ? $data['safe_description'] : '';
  $link = isset($data['safe_link']) ? $data['safe_link'] : '';
  $color = isset($data['safe_color']) ? $data['safe_color'] : '';

  switch ($type) {
    case MEDIAREF_IMAGE_TYPE:
      $filepath = $node->{MEDIAREF_IMAGE_THUMBNAIL}[0]['filepath'];
      break;
    case MEDIAREF_VIDEO_TYPE:
      $filepath = $node->{MEDIAREF_VIDEO_THUMBNAIL}[0]['filepath'];
      break;
    case MEDIAREF_SLIDESHOW_TYPE:
      $filepath = $node->{MEDIAREF_SLIDESHOW_THUMBNAIL}[0]['filepath'];
      break;
  }

  $thumbnail = theme('imagecache', MEDIAREF_IMAGECACHE_THUMB_PROFILE, $filepath);

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