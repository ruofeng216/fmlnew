window.onload = function() {
                var baseUrl = "ws://localhost:9999";
                console.log("Connecting to WebSocket server at " + baseUrl + ".");
                var socket = new WebSocket(baseUrl);

                socket.onclose = function()
                {
                    console.error("web channel closed");
                };
                socket.onerror = function(error)
                {
                    console.error("web channel error: " + error);
                };
                socket.onopen = function()
                {
                    console.log("WebSocket connected, setting up QWebChannel.");
                    new QWebChannel(socket, function(channel) {
                        // make dialog object accessible globally
                        window.CallCpp = channel.objects.CallCpp;
						console.log("web channel OK.");
                    });
                }
            }