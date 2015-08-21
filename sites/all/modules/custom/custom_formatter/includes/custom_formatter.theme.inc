<?php

/**
 * Theme function for a custom_formatter
 */
function theme_custom_formatter_qtip($vars) {
  $output  = '<span class="qtip-link">';
  $output .=    filter_xss_admin($vars['content']);
  $output .= '</span>';
  $output .= '<div class="qtip-tooltip"' . (isset($vars['title']) ? ' data-qtip-title="' . filter_xss_admin($vars['title']) . '"' : '') . (isset($vars['instance']) ? ' data-qtip-instance="'. $vars['instance'] . '"' : '') . '>' .
                $vars['tooltip'] .
             '</div>';
  return $output;
}