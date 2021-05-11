# -*- coding: utf-8 -*-

import requests
import functools
from .config import getServerApiUrl, defaultRequestHeader
from data import SessionModel


class HttpStatusError(Exception):
    """ others http status """
    def __init__(self, status):
        super(HttpStatusError, self).__init__()
        self.status = status

    def __str__(self):
        return "Http status {}".format(self.status)


class HttpInternalServerError(HttpStatusError):
    """ http 500 """
    def __init__(self):
        super(HttpInternalServerError, self).__init__(500)

    def __str__(self):
        return "Http status 500: Internal Server Error"


def wrap_restful(func):
    @functools.wraps(func)
    def wrap_func(*args, **kwargs):
        try:
            r = func(*args, **kwargs)
            if r.status_code in (200, 400):
                return r.json()
            elif r.status_code == 500:
                raise HttpInternalServerError()
            else:
                raise HttpStatusError(r.status_code)
        except Exception as e:
            raise e

    return wrap_func


@wrap_restful
def get(url, params=None, **kwargs):
    return requests.get(url, params, **kwargs)


@wrap_restful
def post(url, data=None, json=None, **kwargs):
    return requests.post(url, data, json, **kwargs)


@wrap_restful
def delete(url, **kwargs):
    return requests.delete(url, **kwargs)


@wrap_restful
def patch(url, data=None, **kwargs):
    return requests.patch(url, data, **kwargs)


@wrap_restful
def put(url, data=None, **kwargs):
    return requests.put(url, data, **kwargs)


class RestErrorCode:
    INVALID_ENUM_VALUE = 10000
    MISS_TOKEN = 10001
    UNAUTHORIZED = 10002
    AUTHORIZE_EXPIRED = 10003


def login(session: SessionModel):
    url = getServerApiUrl(session.host, session.port, "user", "login")
    headers = defaultRequestHeader()
    res = post(url, headers=headers, json={"user": session.user, "password": session.password})
    session.token = res["token"]
    session.userid = res["userid"]
    return session


def session_get(session: SessionModel, service, api,  params=None, **kwargs):
    def inner_get(session: SessionModel, service, api,  params, **kwargs):
        url = getServerApiUrl(session.host, session.port, service, api)
        headers = defaultRequestHeader()
        headers["hku_token"] = session.token
        r = get(url, headers=headers, params=params, **kwargs)
        if "update_token" in r:
            session.token = r["update_token"]
        return r
    if not session.token:
        session = login(session)
    res = inner_get(session, service, api, params, **kwargs)
    if not res["result"] and res['errcode'] == RestErrorCode.AUTHORIZE_EXPIRED:
        session = login(session)
        res = inner_get(session, service, api, params, **kwargs)
    return res

