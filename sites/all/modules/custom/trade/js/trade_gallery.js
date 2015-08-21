(function ($) {

    Drupal.behaviors.tradeGallery = {
        attach: function (context, settings) {
            $('.trade-product-images-gallery', context).once().each(function () {

                var thisGallery = $(this);
                var thisGallerySlideId = '#' + $('.trade-product-images-gallery-slider', thisGallery).attr('id');
                var thisGalleryCarousel = $('.trade-product-images-gallery-carousel', thisGallery);

                $(window).load(function () {
                    $('.trade-product-gallery-nav', thisGallery).ready(function () {
                            $('.slideshow', thisGallery).cycle();
                        }
                    );
                });

                // hide carousel arrow
                var carouselVisible = $('.trade-product-gallery-nav', thisGalleryCarousel).attr('data-cycle-carousel-visible');
                var carouselElementAll = $('.trade-product-gallery-nav img', thisGalleryCarousel).length;


                if (carouselElementAll <= carouselVisible) {
                    $('.trade-gallery-carousel-nav-wr', thisGalleryCarousel).hide();
                }

                $(thisGallerySlideId).flexslider({
                    slideshow: false,
                    animation: "fade",
                    easing: "swing",
                    controlNav: true,
                    directionNav: false,
                    manualControls: ".trade-product-images-gallery-carousel .trade-product-gallery-nav-link"
                });

                // activate magnifier
                $('a.trade-product-image-popup-magnifier', thisGallery).live('click', function () {
                    $('a.colorbox', $(this).parent()).trigger('click');
                    return false;
                });
            });

        }

    };

    // cycle include
    $(window).load(function () {
            $.fn.cycle.defaults.autoSelector = '.slideshow';
        }
    )
})(jQuery);