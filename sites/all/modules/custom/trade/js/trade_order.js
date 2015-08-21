(function ($) {

    Drupal.behaviors.tradeOrder = {
        attach: function (context, settings) {
            var waitTag = $('<div class="trade-ajax-load-overlay"></div><span class="trade-ajax-load-process-wait"></span>');

            // Order ajax actions
            function tradeOrderInit() {
                $('.trade-order-container').once().each(function () {
                    var thisOrder = $(this);
                    $('.trade-order-product-data input:text', thisOrder).each(function () {
                        if ($(this).attr('data-def-value') != $(this).val()) {
                            $(this).val($(this).attr('data-def-value'));
                        }
                    });

                    $('.trade-order-product-data input:not(.form-submit), .trade-order-service-data input:not(.form-submit)', thisOrder).bind('keypress', function (event) {
                        if (event.which == 13) {
                            event.preventDefault();
                            $(this).trigger('blur');
                        }
                    });
                    $('.trade-order-product-data input:not(.form-submit), .trade-order-service-data select, .trade-order-service-data input:not(.form-submit)', thisOrder).bind('change', function (e) {

                        var link = '/trade/order/' + thisOrder.attr('data-order-id') + '/action-edit';
                        var thisValue = $(this).val();

                        var dataFieldName = $(this).attr('data-field-name');
                        var dataOrderLineId = $(this).attr('data-order-line-id');
                        var dataOrderLineType = $(this).attr('data-order-line-type');
                        var dataOrderLineOperation = $(this).attr('data-order-line-operation');

                        if ($(this).hasClass('trade-spinner') && parseInt(thisValue, 10) == thisValue && parseInt(thisValue, 10) > 0 || !$(this).hasClass('trade-spinner')) {
                            $('.trade-order-product-data input', thisOrder).attr('disabled', true);
                            $('.trade-order-product-data a.trade-spinner-button', thisOrder).addClass('trade-disabled');

                            var cloneWait = waitTag.clone();
                            thisOrder.addClass('trade-ajax-load-process');
                            thisOrder.prepend(cloneWait);
                            $.ajax({
                                type: 'POST',
                                data: {
                                    trade_value: thisValue,
                                    trade_field_name: dataFieldName,
                                    trade_order_line_id: dataOrderLineId,
                                    trade_order_line_type: dataOrderLineType,
                                    trade_order_line_operation: dataOrderLineOperation
                                },
                                url: link,
                                success: function (response) {
                                    if (response.status) {
                                        if (response.order) {
                                            thisOrder.replaceWith(response.order);
                                            $(document).trigger('documentChange');
                                            $('.trade-order-container').fadeIn();
                                        }

                                        //update cart block
                                        if (response.cart != undefined && response.cart) {
                                            $('.trade-cart-block-inner:first').replaceWith(response.cart);
                                        }
                                        cloneWait.remove();
                                        thisOrder.removeClass('trade-ajax-load-process');
                                    }

                                    // open all
                                    cloneWait.remove();
                                    thisOrder.removeClass('trade-ajax-load-process');

                                     $('.trade-order-product-data input', thisOrder).attr('disabled', false);
                                    $('.trade-order-product-data a.trade-spinner-button', thisOrder).removeClass('trade-disabled');
                                },
                                error: function (xmlhttp) {
                                    cloneWait.remove();
                                    thisOrder.removeClass('trade-ajax-load-process');
                                    $('.trade-ajax-load-process-wait', thisOrder).remove();
                                    // alert(Drupal.ajaxError(xmlhttp, link));
                                }
                            });
                        }

                    });

                });

            };

            tradeOrderInit();

            $(document).bind('documentChange', function () {
                tradeOrderInit();
            });
        }
    };

})(jQuery);