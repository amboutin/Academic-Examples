# Programmer: Aaron Boutin
# Cites: Much help came from WebApp2 documentation, StackOverFlow, and developers.google.com

from google.appengine.ext import ndb
import webapp2
import json
import logging
from random import randint
from google.appengine.api import urlfetch
import urllib
from webapp2_extras import sessions

import os

class Note(ndb.Model):
	account = ndb.StringProperty(required=True)
	id = ndb.StringProperty()
	name = ndb.StringProperty(required=True)
	note = ndb.StringProperty()
	type = ndb.StringProperty()
	binder = ndb.StringProperty()

class Binder(ndb.Model):
	account = ndb.StringProperty(required=True)
	id = ndb.StringProperty()
	name = ndb.StringProperty()
	type = ndb.StringProperty()
	current_notes = ndb.StringProperty(repeated=True)


class BaseHandler(webapp2.RequestHandler):
    def dispatch(self):
        self.session_store = sessions.get_store(request=self.request)

        try:
            webapp2.RequestHandler.dispatch(self)
        finally:
            self.session_store.save_sessions(self.response)

    @webapp2.cached_property
    def session(self):
        return self.session_store.get_session()

def thirdParty(self):
	string = self.request.headers["Authorization"]			
	url = 'https://www.googleapis.com/plus/v1/people/me'
	token = "Bearer " + string
	try:
		logging.warning("In Try of thirdparty")
		result = urlfetch.fetch(url=url, headers={"Authorization" : token})
		string = json.loads(result.content) 
		logging.warning(result.status_code)				
		if result.status_code == 200:
			logging.warning("Are we getting here?")
			return string["id"]
	except urlfetch.Error:
		logging.exception('Caught exception fetching url')
		return None

#################################################################################################################
# In each request, the access token is sent off to Google+ to get unique userid for use in database
# Each time a resource is acquired, it is first compared to the id from the token use to ensure that use is allowed to access it

#################################################################################################################
class NoteHandler(BaseHandler):

	def post(self):
		uniqueid = thirdParty(self) 
		note_data = json.loads(self.request.body)
		new_note = Note(account=uniqueid, name=note_data['name'], type=note_data['type'], note=note_data['note'], binder=note_data['binder'])		
		new_note.put()
		new_note.id = new_note.key.urlsafe()
		new_note.put()																										
		note_dict = new_note.to_dict()
		note_dict['self'] = '/notes/' + new_note.key.urlsafe()
		note_dict['id'] = new_note.key.urlsafe()
		self.response.write(json.dumps(note_dict))

	def get(self, id=None):
		uniqueid = thirdParty(self)
		q = Note.query(Note.account == uniqueid)
		result = []
		for entry in q:
			dict = {}
			dict['name'] = entry.name
			dict['type'] = entry.type
			dict['note'] = entry.note
			dict['binder'] = entry.binder
			result.append(dict)
		self.response.write(json.dumps(result))	

#################################################################################################################
class ByIdNoteHandler(BaseHandler):

	def get(self, id=None):
		uniqueid = thirdParty(self)
		if id:
			b = ndb.Key(urlsafe=id).get() 
			if b.account == uniqueid:
				b_d = b.to_dict()
				b_d['self'] = "/notes/" + id
				self.response.write(json.dumps(b_d))
			else:
				self.response.write("Cannot Access Information")	

	def patch(self, id=None):
		uniqueid = thirdParty(self)
		if id:
			b = ndb.Key(urlsafe=id).get()
			if b.account == uniqueid:
				patch_note_data = json.loads(self.request.body) 
				if patch_note_data['name']:
					b.name = patch_note_data['name']
				if patch_note_data['type']:
					b.type = patch_note_data['type']
				if patch_note_data['note']:
					b.note = patch_note_data['note']
				if patch_note_data['binder']:
					b.binder = patch_note_data['binder']
				else:
					b.binder = None
					if b.binder == None:
						q = Binder.query(getattr(Binder, "current_notes") == b.id)
						if q:
							for entry in q:
								Binder_key = entry.id
								s = ndb.Key(urlsafe=Binder_key).get()
								s.current_notes.remove(b.id)
								s.put()
				b.put()
				note_dict = b.to_dict()
				note_dict['self'] = '/notes/' + b.key.urlsafe()
				note_dict['id'] = b.key.urlsafe()
				self.response.write(json.dumps(note_dict))
			else:
				self.response.write("Cannot Access Information")

	def delete(self, id=None):
		uniqueid = thirdParty(self)
		if id:
			b = ndb.Key(urlsafe=id).get()
			if b.account == uniqueid:
				if b.binder != None:
					q = Binder.query(getattr(Binder, "current_notes") == b.id)
					for entry in q:
						Binder_key = entry.id
					s = ndb.Key(urlsafe=Binder_key).get()
					s.current_notes.remove(b.id) #= None
					s.put()
				b.key.delete()
			else:
				self.response.write("Cannot Access Information")			
 
	def put(self, id=None):
		uniqueid = thirdParty(self)
		if id:
			b = ndb.Key(urlsafe=id).get()
			if b.account == uniqueid:
				patch_note_data = json.loads(self.request.body)
				b.name = patch_note_data['name']	
				if patch_note_data['type']:
					b.type = patch_note_data['type']
				else:
					b.type = None
				if patch_note_data['note']:
					b.note = patch_note_data['note']
				else:
					b.note = None
				if patch_note_data['binder']:
					b.binder = patch_note_data['binder']
				else:
					b.binder = None
				b.id = b.key.urlsafe()				
				b.put()
				note_dict = b.to_dict()
				note_dict['self'] = '/notes/' + b.key.urlsafe()
				note_dict['id'] = b.key.urlsafe()
				self.response.write(json.dumps(note_dict))
			else:
				self.response.write("Cannot Access Information")
						
#################################################################################################################

class NoteAndBinderHandler(BaseHandler):
	def get(self, BinderID=None, noteID=None):
		uniqueid = thirdParty(self)
		if BinderID:
			if noteID == None:
				s = ndb.Key(urlsafe=BinderID).get()
				if s.account == uniqueid:
					b_d_list = []
					for notes in s.current_notes:
						b = ndb.Key(urlsafe=notes).get()
						b_d = b.to_dict()
						b_d_list.append(b_d)
					self.response.write(json.dumps(b_d_list))
				else:
					b = ndb.Key(urlsafe=noteID).get()
					if b.account == uniqueid:
						b_d = b.to_dict()
						b_d['self'] = "/Binders/" + BinderID + "/" + noteID
						self.response.write(json.dumps(b_d))

	def patch(self, BinderID=None, noteID=None):
		uniqueid = thirdParty(self)
		if BinderID and noteID: 
			Binder = ndb.Key(urlsafe=BinderID).get()
			note = ndb.Key(urlsafe=noteID).get()
			if Binder.account == uniqueid:
				Binder.current_notes.append(note.id)
				note.binder=BinderID
				Binder.put()
				note.put()
				s_d = Binder.to_dict()
				s_d['self'] = "/Binders/" + BinderID + "/" +noteID
				self.response.write(json.dumps(s_d))

#################################################################################################################
class BinderHandler(BaseHandler):
	def post(self):
		uniqueid = thirdParty(self)
		binder_data = json.loads(self.request.body)
		new_binder = Binder(account=uniqueid, current_notes=[], name=binder_data['name'], type=binder_data['type'])
		new_binder.put()
		new_binder.id = new_binder.key.urlsafe()
		new_binder.put()	
		binder_dict = new_binder.to_dict()
		binder_dict['self'] = '/Binders/' + new_binder.key.urlsafe()
		binder_dict['id'] = new_binder.key.urlsafe()
		self.response.write(json.dumps(binder_dict))

	def get(self, id=None):
		uniqueid = thirdParty(self)
		if id:
			s = ndb.Key(urlsafe=id).get()
			if s.account == uniqueid:
				s_d = s.to_dict()
				s_d['self'] = "/Binders/" + id
				self.response.write(json.dumps(s_d))
		else:
			q = Binder.query(Binder.account == uniqueid)
			result = []
			for entry in q:
				if entry.account == uniqueid:
					dict = {}
					dict['name'] = entry.name
					dict['type'] = entry.type
					dict['current_notes'] = entry.current_notes
					dict['id'] = entry.id
					result.append(dict)
			self.response.write(json.dumps(result))

	def delete(self, id=None):
		uniqueid = thirdParty(self)
		if id:
			s = ndb.Key(urlsafe=id).get()
			if s.account == uniqueid:
				if s.current_notes:
					for note in s.current_notes:
						q = Note.query(getattr(Note, "binder") == s.id)
						for entry in q:
							note_key = entry.id
						b = ndb.Key(urlsafe=note_key).get()
						b.binder = None
						b.put()
				s.key.delete()

	def patch(self, id=None):
		uniqueid = thirdParty(self)
		if id:
			s = ndb.Key(urlsafe=id).get()
			if s.account == uniqueid:
				patch_binder_data = json.loads(self.request.body)
				if patch_binder_data['name']:
					s.name = patch_binder_data['name']
				if patch_binder_data['current_notes']:
					s.current_notes = patch_binder_data['current_notes']
				if patch_binder_data['type']:
					s.type = patch_binder_data['type']
				s.put()
				binder_dict = s.to_dict()
				binder_dict['self'] = '/notes/' + s.key.urlsafe()
				binder_dict['id'] = s.key.urlsafe()
				self.response.write(json.dumps(binder_dict))	

	def put(self, id=None):
		uniqueid = thirdParty(self)
		if id:
			s = ndb.Key(urlsafe=id).get()
			if s.account == uniqueid:
				patch_binder_data = json.loads(self.request.body)
				s.name = patch_binder_data['name']	
				if patch_binder_data['current_notes']:
					s.current_notes = patch_binder_data['current_notes']
				else:
					s.current_notes = []
				if patch_binder_data['type']:
					s.type = patch_binder_data['type']
				else:
					s.type = None
				s.id = s.key.urlsafe()				
				s.put()
				binder_dict = s.to_dict()
				binder_dict['self'] = '/notes/' + s.key.urlsafe()
				binder_dict['id'] = s.key.urlsafe()
				self.response.write(json.dumps(binder_dict))

#######################################################################################################################################################################

class LoginPage(BaseHandler):
	def get(self):
		self.session['oauth'] = False
		destination = "https://accounts.google.com/o/oauth2/v2/auth"
		amp = "&"
		eq = "="
		clientID = "client_id=912572776426-7pn8b0fbuuvkfnppjr79cn9upd8c516t.apps.googleusercontent.com"
		secretCode = "TotesASecret" + str(randint(0,99999999))
		state = "state=" + secretCode
		self.session['state'] = secretCode
		redirect = "redirect_uri=http://localhost:8080/redirect"
		uri = destination + "?" + "response_type=code" + amp + "scope=email" + amp + state + amp + clientID + amp + redirect
		result = urlfetch.fetch(uri)
		self.response.out.write(result.content)
		

class RedirectPage(BaseHandler):
	def get(self):
		self.response.out.write("got here")

config = {}
config['webapp2_extras.sessions'] = {
    'secret_key': 'boutina',
}
allowed_methods = webapp2.WSGIApplication.allowed_methods
new_allowed_methods = allowed_methods.union(('PATCH',))
webapp2.WSGIApplication.allowed_methods = new_allowed_methods
app = webapp2.WSGIApplication([
	('/login', LoginPage),
	('/Binders/', BinderHandler),	
	('/Notes/', NoteHandler),
	('/Notes/(.*)', ByIdNoteHandler),
	('/redirect', RedirectPage),
	('/Binders/(.{48})/(.{48})', NoteAndBinderHandler),
	('/Binders/(.{48})/Note', NoteAndBinderHandler),
	('/Binders/(.{48})', BinderHandler)
], debug=True, config=config)