
def make_property( f ):
    """Example usage:

    @make_property
    def name():
        doc = "The crazy awesome name property."
        def fget(self):
            return self._name
        def fset(self, value):
            self._name = value
        def fdel(self):
            del self._name
        return locals()
    """
    return property(**f())
