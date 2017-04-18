import httplib
import time
from twilio.rest import Client

def url_ok(url):
    c = httplib.HTTPConnection(url)
    c.request("HEAD", '')
    print c.getresponse().status


while (1):
    time.sleep(10)
    try:
        url_ok("smartsafe.local")
        print "good"
    except Exception as e:
        print "bad"
        break

account_sid = "xxxxxxxxxxxxxxxxx"
auth_token = "xxxxxxxxxxxxxxxxx"
client = Client(account_sid, auth_token)

message = client.api.account.messages.create(to="+xxxxxxxxxxxxxxxxx",
                                             from_="+xxxxxxxxxxxxxxxxx",
                                             body="Alert! Lost connection to safe!")
