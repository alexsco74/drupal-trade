(function ($) {
    $(window).load(function () {
        $('.social-likes').each(function () {
            var likes = $(this);
            if (typeof $(this).attr('data-counters') == 'undefined') {
                $('li', likes).each(function () {
                    if ($('.social-likes__counter', $(this)).length == 0) {
                        $(this).append('<span class="social-likes__counter social-likes__counter_null">0</span>');
                    }
                });
            }
        });
    });
})(jQuery)
