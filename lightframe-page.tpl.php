<?php
// $Id: $

/**
 * @file
 * Template file for a Modal Frame based on jQuery UI dialog.
 *
 * This template provides the same exact variables provided to page.tpl.php,
 * and serves the same purpose, with the exeption that this template does not
 * render regions such as head, left and right because the main purpose of this
 * template is to render a frame that is displayed on a modal jQuery UI dialog.
 *
 * @see modalframe_theme_registry_alter()
 * @see modalframe_preprocess_page()
 * @see template_preprocess_page()
 * @see template_preprocess()
 * @see theme()
 */

?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language; ?>" lang="<?php print $language->language; ?>" dir="<?php print $language->dir; ?>">
<head>
<?php print $head; ?>
<title><?php print (!empty($title) ? strip_tags($title) : $head_title); ?></title>
<?php print $styles; ?>
<?php print $scripts; ?>
</head>
<body class="lightframe-modal">
<?php if (!empty($navigate_back_url)) { ?>
  <div class="lightframe-navigate-back"><a href="<?php print $navigate_back_url; ?>" title="Navigate back">&lt;&lt;&lt;&nbsp;Back</a></div>
<?php } ?>
<?php if (!empty($title)) { ?>
  <h2><?php print $title; ?></h2>
<?php } ?>
<?php if ($show_messages && $messages) print $messages; ?>
<?php print $help; ?>
<div class="clear-block">
  <?php print $content; ?>
</div>
<?php if (!empty($navigate_back_url)) { ?>
  <div class="lightframe-navigate-back"><a href="<?php print $navigate_back_url; ?>" title="Navigate back">&lt;&lt;&lt;&nbsp;Back</a></div>
<?php } ?>
<?php print $closure; ?>
</body>
</html>